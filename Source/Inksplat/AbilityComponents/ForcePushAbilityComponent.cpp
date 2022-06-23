// Fill out your copyright notice in the Description page of Project Settings.


#include "ForcePushAbilityComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UForcePushAbilityComponent::UForcePushAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}


// Called when the game starts
void UForcePushAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
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
	UE_LOG(LogTemp, Warning, TEXT("Executing Ability"));
	UE_LOG(LogTemp, Warning, TEXT("Owner is %s"), *GetOwner()->GetName());
	if (bCanUseAbility)
	{
		ServerExecuteAbility();
	}
	return;
}

void UForcePushAbilityComponent::ServerExecuteAbility_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Executing on server"));
	//TODO do physics things to objects on server here.
	TArray<APawn*> AffectedPawns = FindAffectedPawns();
	////start cooldown
	bCanUseAbility = false;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_AbilityCooldown,
		this,
		&UForcePushAbilityComponent::ResetAfterCoolDown,
		CoolDownTime,
		false
	);

}

TArray<APawn*> UForcePushAbilityComponent::FindAffectedPawns()
{
	UE_LOG(LogTemp, Warning, TEXT("Finding Affected Pawns"));
	TArray<APawn*> AffectedPawns;
	TArray<FHitResult> OutHits;
	FVector StartPosition = GetOwner()->GetActorLocation() + 
		GetOwner()->GetActorForwardVector() * BoxTraceOffset;
	FVector EndPosition = StartPosition + FVector(0.1, 0.1, 0.1);
	FRotator OwnerRotation = GetOwner()->GetActorRotation();
	UKismetSystemLibrary::BoxTraceMultiByProfile(
		GetWorld(),
		StartPosition,
		EndPosition,
		FVector(BoxLength, BoxWidth, BoxHeight),
		FRotator(0, OwnerRotation.Yaw, 0),
		FName("Pawn"),
		false,
		TArray<AActor*>{GetOwner()},
		EDrawDebugTrace::ForDuration,
		OutHits,
		true
	);

	for (FHitResult HitResult : OutHits)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Actor %s"), *HitResult.Actor->GetName());
	}
	return AffectedPawns;
}

void UForcePushAbilityComponent::ResetAfterCoolDown()
{
	UE_LOG(LogTemp, Warning, TEXT("Cooldown is over"));
	bCanUseAbility = true;
}

void UForcePushAbilityComponent::OnRep_CanUseAbility()
{
	//TODO update UI from this method.
	//bCanUseAbility = bCanUseAbility;
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