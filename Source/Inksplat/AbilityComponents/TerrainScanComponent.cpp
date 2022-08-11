// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainScanComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../PlayerActor/PlayerCharacter.h"
#include "../PaintableGeometry/PaintableActorBase.h"
#include "Net/UnrealNetwork.h"

UTerrainScanComponent::UTerrainScanComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

void UTerrainScanComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTerrainScanComponent::ServerExecuteAbility_Implementation()
{
	TArray<APaintableActorBase*> ActorsWithinRange = GetScannableActors();
	ScanActors(ActorsWithinRange);
	BeginCoolDown();
}

TArray<APaintableActorBase*> UTerrainScanComponent::GetScannableActors()
{
	TArray<APaintableActorBase*> ActorsWithinRange{};
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		for (TObjectIterator<APaintableActorBase> Itr; Itr; ++Itr)
		{
			ActorsWithinRange.Add(*Itr);
		}
	}

	return ActorsWithinRange;
}


void UTerrainScanComponent::ScanActors_Implementation(const TArray<APaintableActorBase*>& ActorsToScan)
{
	UE_LOG(LogTemp, Warning, TEXT("Num actors are %i"), ActorsToScan.Num());
	for (APaintableActorBase* ActorToScan : ActorsToScan)
	{
		if (ActorToScan != nullptr)
		{
			ActorToScan->ScanActor(ScanSpeed, Range);
		}
	}
}