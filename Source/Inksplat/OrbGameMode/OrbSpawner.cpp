// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbSpawner.h"
#include "PaintOrb.h"
#include "../PaintableGeometry/PaintableComponents/PaintableStaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AOrbSpawner::AOrbSpawner()
{
	SpawnerMesh = CreateDefaultSubobject<UPaintableStaticMeshComponent>(TEXT("SpawnerMesh"));
	RootComponent = SpawnerMesh;

	OrbLocation = CreateDefaultSubobject<USceneComponent>(TEXT("OrbLocation"));
	OrbLocation->SetupAttachment(SpawnerMesh);

	bReplicates = true;
}

void AOrbSpawner::SpawnOrb()
{
	//Spawn Orb in predetermined location in server
	if (GetLocalRole() == ROLE_Authority)
	{
		FVector SpawnLocation = OrbLocation->GetComponentLocation();

		Orb = GetWorld()->SpawnActor<APaintOrb>(APaintOrb::StaticClass());
	}
}

void AOrbSpawner::ActivateOrb()
{
	if (GetLocalRole() == ROLE_Authority && Orb != nullptr)
	{
		Orb->Activate();
	}
}