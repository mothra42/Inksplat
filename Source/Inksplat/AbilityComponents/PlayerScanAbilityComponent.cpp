// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerScanAbilityComponent.h"
#include "Net/UnrealNetwork.h"
#include "../PlayerActor/PlayerCharacter.h"

UPlayerScanAbilityComponent::UPlayerScanAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

void UPlayerScanAbilityComponent::ServerExecuteAbility_Implementation()
{
	//Find players in server
	TArray<APlayerCharacter*> PlayersWithinRange = GetPlayerPawnsWithinRadius();

	//Scan those players
	RevealPlayers(PlayersWithinRange);
}

TArray<APlayerCharacter*> UPlayerScanAbilityComponent::GetPlayerPawnsWithinRadius()
{
	TArray<APlayerCharacter*> PlayersWithinRange{};
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		for (TObjectIterator<APlayerCharacter> Itr; Itr; ++Itr)
		{
			float DistanceBetweenPlayers = FVector::Distance(
				GetOwner()->GetActorLocation(), 
				Itr->GetActorLocation()
			);
			if (DistanceBetweenPlayers <= ScanRadius)
			{
				PlayersWithinRange.Add(*Itr);
			}
		}
	}

	return PlayersWithinRange;
}

void UPlayerScanAbilityComponent::RevealPlayers_Implementation(const TArray<APlayerCharacter*>& PlayersToReveal)
{
	//TODO
	//change texture on skeletal mesh to reveal the outline.
	//Design question to answer, do I show a snapshot? or does the effect last as long as the player uses the ability.
	//I'm leaning towards the snapshot method for now, but I will make both to as I think its something that needs to be tested in playtesting.
}

