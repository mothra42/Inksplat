// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbilityComponent.h"
#include "PlayerScanAbilityComponent.generated.h"

/**
 * 
 */
UCLASS()
class INKSPLAT_API UPlayerScanAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()
public:
	UPlayerScanAbilityComponent();

protected:
	//Scan radius is set to a default of 60 meters
	UPROPERTY(Category = "Scan Properties", EditDefaultsOnly)
	float ScanRadius = 6000.0f;

private:
	virtual void ServerExecuteAbility_Implementation() override;
	
	TArray<class APlayerCharacter*> GetPlayerPawnsWithinRadius();

	UFUNCTION(Client, Reliable)
	void RevealPlayers(const TArray<APlayerCharacter*>& PlayersToReveal);
};
