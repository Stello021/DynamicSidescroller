// Copyright Epic Games, Inc. All Rights Reserved.

#include "DynamicSidescrollerGameMode.h"
#include "DynamicSidescrollerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADynamicSidescrollerGameMode::ADynamicSidescrollerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
