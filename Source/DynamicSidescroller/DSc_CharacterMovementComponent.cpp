// Fill out your copyright notice in the Description page of Project Settings.


#include "DSc_CharacterMovementComponent.h"
#include "GameFramework/Character.h"

UDSc_CharacterMovementComponent::UDSc_CharacterMovementComponent()
	: OldFloorSurface(SurfaceType_Default)
{
	// Initialize curve with default values
	const ConstructorHelpers::FObjectFinder<UCurveFloat> DefaultSpeedCurve(
		TEXT("/Game/Characters/Player/Blueprints/Curves/C_SlopeMovementSpeed"));
	if (DefaultSpeedCurve.Succeeded())
	{
		OnSlopeSpeedCurve = DefaultSpeedCurve.Object;
	}

	FSurfaceSettings DefaultSurfaceSettings;
	DefaultSurfaceSettings.GroundFriction = 8.f;
	DefaultSurfaceSettings.BrakingDecelerationOnWalking = 20000.f;

	for (int32 i = 0; i < SurfaceType_Max; i++)
	{
		TEnumAsByte<EPhysicalSurface> SurfaceType(i);

		SurfaceMap.Add(SurfaceType, DefaultSurfaceSettings);
	}
}

void UDSc_CharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	Super::PhysWalking(deltaTime, Iterations);
	CalculateSpeedOnSlope(deltaTime);

	/** Check if current floor surface type is changed*/
	if (!SurfaceSettings.IsEmpty() && OldFloorSurface != CurrentFloor.HitResult.PhysMaterial->SurfaceType)
	{
		OldFloorSurface = CurrentFloor.HitResult.PhysMaterial->SurfaceType;

		/** Get relative settings*/
		const FSurfaceSettings& CurrentSettings = SurfaceSettings[OldFloorSurface];
		BrakingDecelerationWalking = CurrentSettings.BrakingDecelerationOnWalking;
		GroundFriction = CurrentSettings.GroundFriction;
	}
}

void UDSc_CharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	if (SurfaceSettings.IsEmpty())
	{
		BuildRuntimeSurfaceSettings();
	}
}

/** Event called when edit a Property in editor*/
void UDSc_CharacterMovementComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Check for surface settings TMap property
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UDSc_CharacterMovementComponent, SurfaceMap))
	{
		//Rebuild TArray
		BuildRuntimeSurfaceSettings();
	}
}

void UDSc_CharacterMovementComponent::BuildRuntimeSurfaceSettings()
{
	//Emptied from old values
	SurfaceSettings.Empty();

	// //Init Surface Settings Array
	SurfaceSettings.SetNum(SurfaceMap.Num());


	for (const auto& Pair : SurfaceMap)
	{
		SurfaceSettings.EmplaceAt(Pair.Key, Pair.Value);
	}
}

void UDSc_CharacterMovementComponent::CalculateSpeedOnSlope(const float deltaTime)
{
	if (!OnSlopeSpeedCurve && !GetCharacterOwner())
	{
		return;
	}
	const float SlopeAlignement = FVector::DotProduct(CurrentFloor.HitResult.ImpactNormal,
	                                                  GetCharacterOwner()->GetActorForwardVector());
	const float FixedSpeed = OnSlopeSpeedCurve->GetFloatValue(SlopeAlignement);
	MaxWalkSpeed = FMath::FInterpTo(MaxWalkSpeed, FixedSpeed, deltaTime, 20.f);
}
