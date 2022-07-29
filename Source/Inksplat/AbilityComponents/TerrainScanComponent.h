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
	virtual void BeginPlay();

	//Methods and variables inherited from interface
protected:
	virtual void ResetAfterCoolDown() override;

	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteAbility_BPTest();
private:
	UFUNCTION(Server, Reliable)
	void ServerExecuteAbility();

public:
	virtual void UseAbility() override;
};
