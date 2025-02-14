// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "DSc_CharacterMovementComponent.generated.h"

class UCurveFloat;
struct FFindFloorResult;

/** Surface settings to apply on that component*/
USTRUCT(BlueprintType)
struct FSurfaceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float BrakingDecelerationOnWalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float GroundFriction;
	
};

/**
 * 
 */
UCLASS()
class DYNAMICSIDESCROLLER_API UDSc_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UDSc_CharacterMovementComponent();

	/**Curve that represent a max walk speed value at a certain slope value*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Slopes)
	UCurveFloat* OnSlopeSpeedCurve;

protected:

	//Collect all different surfaces and relative settings to apply
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking (Surface Settings)")
	TMap<TEnumAsByte<EPhysicalSurface>, FSurfaceSettings> SurfaceMap;

	TEnumAsByte<EPhysicalSurface> OldFloorSurface;

	/**copy of Surface Settings Tmap for optimized runtime usage*/
	TArray<FSurfaceSettings> SurfaceSettings;
	
	virtual void PhysWalking(float deltaTime, int32 Iterations) override;

	virtual  void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:

	/** Build the TArray associated to Surface Settings TMap */
	void BuildRuntimeSurfaceSettings();
	
	void CalculateSpeedOnSlope(float deltaTime);
	
	
};
