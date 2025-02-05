// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerPath.generated.h"

class USplineComponent;

UCLASS()
class DYNAMICSIDESCROLLER_API APlayerPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerPath();

	/** Spline that indicates path to follow*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USplineComponent* SplineComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
