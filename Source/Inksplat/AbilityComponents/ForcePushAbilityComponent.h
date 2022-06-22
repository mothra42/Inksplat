// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseAbilityComponent.h"
#include "ForcePushAbilityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INKSPLAT_API UForcePushAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UForcePushAbilityComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION(Server, Reliable)
	void ServerExecuteAbility();

	TArray<APawn*> FindAffectedPawns();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CanUseAbility)
	bool bCanUseAbility = true;

	UPROPERTY(Category = "Cool Down Settings", EditDefaultsOnly)
	float CoolDownTime = 0.5;

	FTimerHandle TimerHandle_AbilityCooldown;

	UFUNCTION()
	void OnRep_CanUseAbility();


//Methods and variables inherited from interface
protected:
	virtual void ResetAfterCoolDown() override;

public:
	virtual void UseAbility() override;
	
};
