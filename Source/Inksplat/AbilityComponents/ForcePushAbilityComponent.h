// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseAbilityComponent.h"
#include "ForcePushAbilityComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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

	TSet<APawn*> FindAffectedPawns();

	void ApplyImpulseToPawns(const TSet<APawn*>& AffectedPawns);

	FVector CalculateForceToApply(FVector PawnLocation);

	void ApplyTempPaintToHitPlayer(class APlayerCharacter* OtherPlayer);

	//TODO make the color initialization work
	
	//void InitializeTempColor(const FColor ColorToSet);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CanUseAbility)
	bool bCanUseAbility = true;

	UPROPERTY(Category = "Cool Down Settings", EditDefaultsOnly)
	float CoolDownTime = 0.5;

	FTimerHandle TimerHandle_AbilityCooldown;

	UFUNCTION()
	void OnRep_CanUseAbility();
protected:
	//sweep properties
	UPROPERTY(Category = "Box Trace Settings", EditDefaultsOnly)
	float BoxLength = 250.0;

	UPROPERTY(Category = "Box Trace Settings", EditDefaultsOnly)
	float BoxWidth = 400.0;

	UPROPERTY(Category = "Box Trace Settings", EditDefaultsOnly)
	float BoxHeight = 100.0;

	UPROPERTY(Category = "Box Trace Settings", EditDefaultsOnly)
	float BoxTraceOffset = 300.0;

	//Force properties
	UPROPERTY(Category = "Force Settings", EditDefaultsOnly)
	float BaseForce = 30000000.0;

	//Force scales with 1/Radius, so smaller numbers make a larger force
	UPROPERTY(Category = "Force Settings", EditDefaultsOnly)
	float MinRadiusCorrection = 200.f;

	//Force scales with 1/Radius, so smaller numbers make a larger force
	UPROPERTY(Category = "Force Settings", EditDefaultsOnly)
	float MaxRadiusCorrection = 240.f;

//Methods and variables inherited from interface
protected:
	virtual void ResetAfterCoolDown() override;

public:
	virtual void UseAbility() override;
	
};
