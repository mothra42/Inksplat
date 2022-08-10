// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbilityComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UBaseAbilityComponent::UBaseAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}


// Called when the game starts
void UBaseAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UBaseAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBaseAbilityComponent, bCanUseAbility);
}

void UBaseAbilityComponent::UseAbility()
{
	if (bCanUseAbility)
	{
		ServerExecuteAbility();
	}
}

void UBaseAbilityComponent::ServerExecuteAbility_Implementation()
{
	return;
}

void UBaseAbilityComponent::EndAbilityUse()
{
	return;
}

void UBaseAbilityComponent::BeginCoolDown()
{
	bCanUseAbility = false;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_AbilityCooldown,
		this,
		&UBaseAbilityComponent::ResetAfterCoolDown,
		CoolDownTime,
		false
	);
}

void UBaseAbilityComponent::ResetAfterCoolDown()
{
	bCanUseAbility = true;
}

void UBaseAbilityComponent::OnRep_CanUseAbility()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling base on rep"));
	return;
}