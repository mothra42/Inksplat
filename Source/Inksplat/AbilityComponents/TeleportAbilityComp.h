// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbilityComponent.h"
#include "TeleportAbilityComp.generated.h"

/**
 * 
 */
UCLASS()
class INKSPLAT_API UTeleportAbilityComp : public UBaseAbilityComponent
{
	GENERATED_BODY()
	
public:
	UTeleportAbilityComp();

protected:
	UPROPERTY(Category = "Teleport Settings", EditDefaultsOnly)
	float TeleportRange = 1000.f;

	UPROPERTY(Category = "Teleport Settings", EditDefaultsOnly)
	float PawnHeightCorrection = 96.f;

private:
	virtual void ServerExecuteAbility_Implementation() override;

	FVector FindTeleportLocation();

	FVector FindSafeTeleportLocation(const FVector TeleportDirection);

	FVector FindCorrectZPlacement(FVector XYLocation);

	//utility method used to line trace and find any blocking geometry
	void LineTraceForGeometry(TArray<FHitResult>& OutHitResults, const FVector StartLocation, const FVector EndLocation);

	void TeleportToLocation(const FVector LocationToTeleportTo);
};
