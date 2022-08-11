// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleportationCardAbilityComp.h"
#include "../PlayerActor/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UTeleportationCardAbilityComp::UTeleportationCardAbilityComp()
{
	SetIsReplicated(true);
}

void UTeleportationCardAbilityComp::ServerExecuteAbility_Implementation()
{
	if (bIsCardActivated)
	{
		TeleportToCard();
	}
	else
	{
		//Spawn mesh that shows trajectory of card on server and client
			//get world location of intersection point on server. 
				//use that point to calculate projectile arc in server
					//fire replicated teleportation card in server.
	}
}

void UTeleportationCardAbilityComp::TeleportToCard()
{
	//Get Card Location

	APlayerCharacter* OwningPlayer = Cast<APlayerCharacter>(GetOwner());
	if (OwningPlayer != nullptr)
	{
		//Confirmed that TeleportTo works perfectly for my use case.
		//OwningPlayer->TeleportTo(CardLocation,)
	}
}