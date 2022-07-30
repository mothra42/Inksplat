// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseAbilityComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INKSPLAT_API UBaseAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseAbilityComponent();

private:
	FTimerHandle TimerHandle_AbilityCooldown;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CanUseAbility)
	bool bCanUseAbility = true;

	UPROPERTY(Category = "Ability Cool Down", EditDefaultsOnly)
	float CoolDownTime = 0.5f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	virtual void ServerExecuteAbility();

	virtual void ResetAfterCoolDown();

	UFUNCTION()
	virtual void OnRep_CanUseAbility();

	virtual void BeginCoolDown();

public:	
	virtual void UseAbility();

	virtual void SetTempPaintColor(const FColor ColorToSet) { return; }

		
};
