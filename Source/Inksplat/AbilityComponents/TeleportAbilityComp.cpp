// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleportAbilityComp.h"
#include "Net/UnrealNetwork.h"

UTeleportAbilityComp::UTeleportAbilityComp()
{
	SetIsReplicated(true);
}

void UTeleportAbilityComp::ServerExecuteAbility_Implementation()
{
	TeleportToLocation(FindTeleportLocation());
}

FVector UTeleportAbilityComp::FindTeleportLocation()
{
	FVector TeleportLocation;
	//Using Forward vector is slightly fraut because it could be pointing up or down.
	TeleportLocation = GetOwner()->GetActorLocation() + 
		(GetOwner()->GetActorForwardVector() * TeleportRange);
	return TeleportLocation;
}

bool UTeleportAbilityComp::IsLocationValid(const FVector LocationToTest)
{
	return false;
	//TODO
	//Check if location overlaps with existing level geometry, players are ok.
	//check if location is within the valid bounds of the level. 
}

void UTeleportAbilityComp::TeleportToLocation(const FVector LocationToTeleportTo)
{
	GetOwner()->TeleportTo(LocationToTeleportTo,
		GetOwner()->GetActorRotation(), 
		false, 
		true
	);
}