// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbilityComponent.h"
#include "TeleportationCardAbilityComp.generated.h"

/**
 * 
 */
UCLASS()
class INKSPLAT_API UTeleportationCardAbilityComp : public UBaseAbilityComponent
{
	GENERATED_BODY()

protected:
	bool bIsCardActivated = false;

public:
	UTeleportationCardAbilityComp();

protected:
	void TeleportToCard();

private:
	virtual void ServerExecuteAbility_Implementation() override;
};
