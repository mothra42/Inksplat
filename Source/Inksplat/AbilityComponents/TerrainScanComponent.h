// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbilityComponent.h"
#include "TerrainScanComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INKSPLAT_API UTerrainScanComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()
	
public:
	UTerrainScanComponent();

protected:
	UPROPERTY(Category = "Scan Properties", EditDefaultsOnly)
	float ScanSpeed = -0.3f;

	UPROPERTY(Category = "Scan Properties", EditDefaultsOnly)
	float Range = 3.0f;

protected:
	virtual void BeginPlay();

private:
	virtual void ServerExecuteAbility_Implementation() override;

	TArray<APaintableActorBase*> GetScannableActors();

	UFUNCTION(Client, Reliable)
	void ScanActors(const TArray<APaintableActorBase*>& ActorsToScan);
};
