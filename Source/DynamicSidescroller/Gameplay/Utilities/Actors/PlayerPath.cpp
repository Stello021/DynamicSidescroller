// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPath.h"
#include "Components/SplineComponent.h"

// Sets default values
APlayerPath::APlayerPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Spline Path Component
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Path"));
	SplineComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void APlayerPath::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

