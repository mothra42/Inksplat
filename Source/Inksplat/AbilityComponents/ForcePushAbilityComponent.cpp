// Fill out your copyright notice in the Description page of Project Settings.


#include "ForcePushAbilityComponent.h"
//#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UForcePushAbilityComponent::UForcePushAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UForcePushAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UForcePushAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UForcePushAbilityComponent, bCanUseAbility);
}


// Called every frame
void UForcePushAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UForcePushAbilityComponent::UseAbility()
{
	//if the ability is useable call server execute ability
	UE_LOG(LogTemp, Warning, TEXT("ExecutingAbility"));
	return;
}

void UForcePushAbilityComponent::ServerExecuteAbility_Implementation()
{
	//TODO do physics things to objects on server here.
}

void UForcePushAbilityComponent::ResetAfterCoolDown()
{
	return;
}

// Force push should work as a simple call to execute a method on the server, much like firing a gun.

//Force push therefore needs an execution method that makes force push happen on server
	//this will need to be a server RPC call

//Force push also needs a cooldown timer that can be adjusted from blueprint for balancing.
	//similar to weapon firing methods

//force push needs several variables to tweak for flexibility
	//The amount of force to apply
	//Valid objects that can be pushed
	//room for animation and VFX
	//A Mesh that looks for objects that need to have force applied to them.

//It would be best if all abilities inherited from a basic ability interface that mandates
//a cool down and an execution method. Thos are the two things every ability should have.