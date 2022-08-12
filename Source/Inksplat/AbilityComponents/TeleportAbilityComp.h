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

private:
	virtual void ServerExecuteAbility_Implementation() override;

	FVector FindTeleportLocation();

	bool IsLocationValid(const FVector LocationToTest);

	void TeleportToLocation(const FVector LocationToTeleportTo);
};
