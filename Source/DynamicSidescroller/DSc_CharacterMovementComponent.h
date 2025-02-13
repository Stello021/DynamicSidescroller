// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DSc_CharacterMovementComponent.generated.h"

class UCurveFloat;

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
	virtual void PhysWalking(float deltaTime, int32 Iterations) override;

	void CalculateSpeedOnSlope(float deltaTime);
	
	
};
