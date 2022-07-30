// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainScanComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../PlayerActor/PlayerCharacter.h"
#include "../PaintableGeometry/PaintableActorBase.h"
#include "../PaintableGeometry/PaintableComponents/PaintableStaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

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
	TArray<APaintableActorBase*> CurrentlyRenderedActors;
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Running in server"));
		//Iterate Over Actors
		for (TObjectIterator<APaintableActorBase> Itr; Itr; ++Itr)
		{
			if (Itr->GetLastRenderTime() < 0.01)
			{
				UE_LOG(LogTemp, Warning, TEXT("Actor In Server is %s"), *Itr->GetName());
				CurrentlyRenderedActors.Add(*Itr);
				Itr->ScanActor(-0.3f);
			}
		}
	
		UE_LOG(LogTemp, Warning, TEXT("Rendering %i actors in view"), CurrentlyRenderedActors.Num());
	}
}
