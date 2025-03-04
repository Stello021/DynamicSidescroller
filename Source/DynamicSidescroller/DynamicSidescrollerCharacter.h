// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "DynamicSidescrollerCharacter.generated.h"

class USplineComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

class APlayerPath;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ADynamicSidescrollerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneComponent* CameraPivot;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;


public:
	ADynamicSidescrollerCharacter(const FObjectInitializer& ObjectInitializer);
	
	/** Path to follow */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
	APlayerPath* Path;

	/** Distance to follow the current path direction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Path)
	float ScanDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CameraInterpolationSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	bool bKeepSidescrollerView;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	FVector CalculateMovementDirection(const USplineComponent& SplineComponent, const float MovementAxisValue) const;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
    /** Determine the target rotation, based on movement direction*/
	FQuat GetTargetPivotQuat(float MovementAxisValue) const;

	/** Called for keeping sidescroller view during movement and direction changes */
	void KeepSidescrollerView(float MovementAxisValue) const;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

