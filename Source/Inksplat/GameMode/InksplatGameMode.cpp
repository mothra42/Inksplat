// Copyright Epic Games, Inc. All Rights Reserved.

#include "InksplatGameMode.h"
#include "../HUD/InksplatHUD.h"
#include "../PlayerActor/PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

AInksplatGameMode::AInksplatGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	//C:/Users/Battlestation One/Desktop/Active Projects/Inksplat/Content/FirstPersonCPP/Blueprints/FirstPersonCharacter.uasset
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/PlayerCharacter_BP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	// use our custom HUD class
	HUDClass = AInksplatHUD::StaticClass();
}
