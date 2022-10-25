// Copyright Epic Games, Inc. All Rights Reserved.

#include "SparrowRPGGameMode.h"
#include "SparrowRPGCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASparrowRPGGameMode::ASparrowRPGGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
