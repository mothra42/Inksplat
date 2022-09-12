// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PaintableGeometry/PaintableActorBase.h"
#include "OrbSpawner.generated.h"

/**
 * 
 */
UCLASS()
class INKSPLAT_API AOrbSpawner : public APaintableActorBase
{
	GENERATED_BODY()

	AOrbSpawner();

protected:
	UPROPERTY(Category = "Orb Setup", EditDefaultsOnly)
	USceneComponent* OrbLocation;

	UPROPERTY(Category = "Mesh", EditDefaultsOnly)
	class UPaintableStaticMeshComponent* SpawnerMesh;

	class APaintOrb* Orb;

public:
	void SpawnOrb();
//public function to spawn an orb, should be called by the game mode class.

//private function to activate orb once paint threshold is breached.
	void ActivateOrb();
};
