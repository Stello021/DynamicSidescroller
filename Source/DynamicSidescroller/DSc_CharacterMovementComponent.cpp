// Fill out your copyright notice in the Description page of Project Settings.


#include "DSc_CharacterMovementComponent.h"

#include "InterchangeResult.h"
#include "GameFramework/Character.h"

UDSc_CharacterMovementComponent::UDSc_CharacterMovementComponent()
: OldFloorSurface(SurfaceType_Default), bAffectedBySlope(true), SlopeSpeedScale(1.f)
{
	// Initialize curve with default values
	const ConstructorHelpers::FObjectFinder<UCurveFloat> DefaultSpeedCurve(
		TEXT("/Game/Characters/Player/Blueprints/Curves/C_SlopeMovementSpeed"));
	if (DefaultSpeedCurve.Succeeded())
	{
		OnSlopeSpeedCurve = DefaultSpeedCurve.Object;
	}

	FSurfaceSettings DefaultSurfaceSettings;
	DefaultSurfaceSettings.GroundFriction = GroundFriction;
	DefaultSurfaceSettings.BrakingDecelerationOnWalking = BrakingDecelerationWalking;
	DefaultSurfaceSettings.MaxWalkingSpeed = MaxWalkSpeed;
	DefaultSurfaceSettings.bAffectedBySlope = bAffectedBySlope;

	for (int32 i = 0; i < SurfaceType_Max; i++)
	{
		TEnumAsByte<EPhysicalSurface> SurfaceType(i);

		FText SurfaceTypeDisplayText;

		UEnum::GetDisplayValueAsText(SurfaceType, SurfaceTypeDisplayText);

		FString ExpectedText = FString::Printf(TEXT("Surface Type %d"), i);

		if (SurfaceTypeDisplayText.ToString() == ExpectedText)
		{
			break;
		}
		SurfaceMap.Add(SurfaceType, DefaultSurfaceSettings);
	}
}

void UDSc_CharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	Super::PhysWalking(deltaTime, Iterations);

	/** Check if current floor surface type is changed*/
	if (!SurfaceSettings.IsEmpty() && OldFloorSurface != CurrentFloor.HitResult.PhysMaterial->SurfaceType)
	{
		OldFloorSurface = CurrentFloor.HitResult.PhysMaterial->SurfaceType;

		/** Get relative settings*/
		const FSurfaceSettings& CurrentSettings = SurfaceSettings[OldFloorSurface];
		BrakingDecelerationWalking = CurrentSettings.BrakingDecelerationOnWalking;
		GroundFriction = CurrentSettings.GroundFriction;
		MaxWalkSpeed = CurrentSettings.MaxWalkingSpeed;
		bAffectedBySlope = CurrentSettings.bAffectedBySlope;
		SlopeSpeedScale = SurfaceSettings[SurfaceType_Default].MaxWalkingSpeed / MaxWalkSpeed;
	}

	if (bAffectedBySlope)
	{
		CalculateSpeedOnSlope(deltaTime);
	}
}

void UDSc_CharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	BuildRuntimeSurfaceSettings();
	SurfaceMap.Empty();
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
	
	float FixedSpeed = OnSlopeSpeedCurve->GetFloatValue(SlopeAlignement);

	if (SlopeSpeedScale > 1.f)
	{
		FixedSpeed /= SlopeSpeedScale;
	}
	else if (SlopeSpeedScale < 1.f)
	{
		FixedSpeed *= SlopeSpeedScale;
	}
	MaxWalkSpeed = FMath::FInterpTo(MaxWalkSpeed, FixedSpeed, deltaTime, 20.f);
}
