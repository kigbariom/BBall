// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BBallGameMode.h"
#include "BBallHUD.h"
#include "BBallCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABBallGameMode::ABBallGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ABBallHUD::StaticClass();
}
