// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerScanAbilityComponent.h"
#include "Net/UnrealNetwork.h"
#include "../PlayerActor/PlayerCharacter.h"
#include "../PaintableGeometry/PaintableComponents/PaintableSkeletalMeshComponent.h"
#include "AbilityActors/PlayerGhost.h"

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
	//Spawn ghost of player in that position, preferably in the same pose as the player at the time of scan.
	for (APlayerCharacter* PlayerToReveal : PlayersToReveal)
	{
		if (PlayerToReveal != nullptr && PlayerToReveal != GetOwner())
		{
			FVector SpawnLocation = PlayerToReveal->GetFullBodyMesh()->GetComponentLocation();
			FRotator SpawnRotation = PlayerToReveal->GetFullBodyMesh()->GetComponentRotation();

			GetWorld()->SpawnActor<APlayerGhost>(PlayerGhostClass, SpawnLocation, SpawnRotation);
		}
	}
}

void UPlayerScanAbilityComponent::EndAbilityUse()
{
	HidePlayers();
	ServerBeginCoolDown();
}

void UPlayerScanAbilityComponent::HidePlayers()
{
	//remove player ghosts
	for (TObjectIterator<APlayerGhost> Itr; Itr; ++Itr)
	{
		Itr->Destroy();
	}
}

void UPlayerScanAbilityComponent::ServerBeginCoolDown_Implementation()
{
	BeginCoolDown();
}