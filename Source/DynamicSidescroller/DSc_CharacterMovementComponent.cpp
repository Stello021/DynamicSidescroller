// Fill out your copyright notice in the Description page of Project Settings.


#include "DSc_CharacterMovementComponent.h"
#include "GameFramework/Character.h"

UDSc_CharacterMovementComponent::UDSc_CharacterMovementComponent()
{
	// Initialize curve with default values
	const ConstructorHelpers::FObjectFinder<UCurveFloat> DefaultSpeedCurve(TEXT("/Game/Characters/Player/Blueprints/Curves/C_SlopeMovementSpeed"));
	if (DefaultSpeedCurve.Succeeded())
	{
		OnSlopeSpeedCurve = DefaultSpeedCurve.Object;
	}
}

void UDSc_CharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	Super::PhysWalking(deltaTime, Iterations);
	CalculateSpeedOnSlope(deltaTime);
	
}

void UDSc_CharacterMovementComponent::CalculateSpeedOnSlope(const float deltaTime)
{
	if (!OnSlopeSpeedCurve && !GetCharacterOwner())
	{
		return;
	}
	const float SlopeAlignement = FVector::DotProduct(CurrentFloor.HitResult.ImpactNormal, GetCharacterOwner()->GetActorForwardVector());
	const float FixedSpeed = OnSlopeSpeedCurve->GetFloatValue(SlopeAlignement);
	MaxWalkSpeed = FMath::FInterpTo(MaxWalkSpeed, FixedSpeed, deltaTime, 20.f);
}
