// Copyright Epic Games, Inc. All Rights Reserved.

#include "DynamicSidescrollerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "VectorTypes.h"
#include "Components/SplineComponent.h"
#include "Gameplay/Utilities/Actors/PlayerPath.h"
#include "DSc_CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ADynamicSidescrollerCharacter

ADynamicSidescrollerCharacter::ADynamicSidescrollerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDSc_CharacterMovementComponent>(TEXT("DScCharMoveComp")))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 1200.f;
	GetCharacterMovement()->AirControl = 1.f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;
	GetCharacterMovement()->FallingLateralFriction = 500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 700.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Init as null the player path
	Path = nullptr;
	ScanDistance = 250.f;

	GetCapsuleComponent()->bReturnMaterialOnMove = true;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADynamicSidescrollerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ADynamicSidescrollerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
		                                   &ADynamicSidescrollerCharacter::Move);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

/** Player moves only in the right direction*/
void ADynamicSidescrollerCharacter::Move(const FInputActionValue& Value)
{
	// input is a float
	const float MovementAxisValue = Value.Get<float>();

	if (Controller != nullptr && Path != nullptr)
	{
		if (!Path->SplineComponent->IsClosedLoop())
		{
			// Get the character's current distance along the spline
			const float CurrentDistance = Path->SplineComponent->GetDistanceAlongSplineAtLocation(GetActorLocation(), ESplineCoordinateSpace::World);

			// Restrict movement at spline boundaries
			if (const float SplineLength = Path->SplineComponent->GetSplineLength();
				(CurrentDistance <= 0.0f && MovementAxisValue < 0) || (CurrentDistance >= SplineLength && MovementAxisValue > 0))
			{
				// Prevent movement if trying to go out of bounds
				return;
			}
		}

		const FVector MovementDirection = CalculateMovementDirection(*Path->SplineComponent, MovementAxisValue);
			
		// add movement
		//Direction is normalized to avoid arbitrary magnitude, Input value is absolute because represent only the intensity
		//positivity is checked previously
		AddMovementInput(MovementDirection.GetSafeNormal2D(), FMath::Abs(MovementAxisValue));
	}
}

FVector ADynamicSidescrollerCharacter::CalculateMovementDirection(const USplineComponent& SplineComponent,
	const float MovementAxisValue) const
{
	//Finding the tangent of the spline's closest point to this Character
	//TANGENT: a vector that touches a specific point of the spline and points to its direction
	//This vector normalized represent the orientation that player use to move right or left from each point of the path
	const FVector ClosestSplineTangent = Path->SplineComponent->FindTangentClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
	//All vectors ignored Z-axis to not be affected by the height distance from player and spline (Z-axis is only for jumping)
	FVector MovementOrientation = ClosestSplineTangent.GetSafeNormal2D();
	//orientation is positive or negative, based on input value, and is followed for an amount based on scan distance
	MovementOrientation = MovementOrientation * MovementAxisValue * ScanDistance;

	//Finding the new spline's point to move towards
	const FVector ClosestNextSplinePoint = Path->SplineComponent->FindLocationClosestToWorldLocation(
		 MovementOrientation + GetActorLocation(), ESplineCoordinateSpace::World);
	const FVector MovementDirection = ClosestNextSplinePoint - GetActorLocation();
	return MovementDirection;
}

