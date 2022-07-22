// Fill out your copyright notice in the Description page of Project Settings.


#include "ForcePushAbilityComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../PlayerActor/PlayerCharacter.h"
#include "../PaintableGeometry/PaintableComponents/PaintableSkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	if (bCanUseAbility)
	{
		ServerExecuteAbility();
	}
	return;
}

void UForcePushAbilityComponent::ServerExecuteAbility_Implementation()
{
	bCanUseAbility = false;
	TSet<APawn*> AffectedPawns = FindAffectedPawns();
	ApplyImpulseToPawns(AffectedPawns);
	////start cooldown

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_AbilityCooldown,
		this,
		&UForcePushAbilityComponent::ResetAfterCoolDown,
		CoolDownTime,
		false
	);

}

TSet<APawn*> UForcePushAbilityComponent::FindAffectedPawns()
{
	TSet<APawn*> AffectedPawns;
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
		EDrawDebugTrace::None,
		OutHits,
		true
	);

	//TODO this could be further optimized, since each physics component counts as a hit.
	for (FHitResult HitResult : OutHits)
	{
		APawn* PawnToAdd = Cast<APawn>(HitResult.Actor);
		if (PawnToAdd != nullptr)
		{
			AffectedPawns.Add(PawnToAdd);
		}
	}
	return AffectedPawns;
}

//TODO give this method a better name like ApplyImpusleAndPaint
void UForcePushAbilityComponent::ApplyImpulseToPawns(const TSet<APawn*>& AffectedPawns)
{
	for (APawn* Pawn : AffectedPawns)
	{
		APlayerCharacter* OtherPlayer = Cast<APlayerCharacter>(Pawn);
		if (OtherPlayer != nullptr)
		{
			ApplyTempPaintToHitPlayer(OtherPlayer);

			FVector ForceToApply = CalculateForceToApply(Pawn->GetActorLocation());
			OtherPlayer->GetCharacterMovement()->AddImpulse(ForceToApply);
		}
	}
}

FVector UForcePushAbilityComponent::CalculateForceToApply(FVector PawnLocation)
{
	FVector ForceOrigin = GetOwner()->GetActorLocation();
	float Radius = FVector::Distance(ForceOrigin, PawnLocation);
	float CorrectedRadius = FMath::Clamp(Radius, MinRadiusCorrection, MaxRadiusCorrection);
	FVector ForceDirection = ((PawnLocation - ForceOrigin) + FVector(0, 0, 100)).GetSafeNormal();
	return ForceDirection * (BaseForce / CorrectedRadius);
}

void UForcePushAbilityComponent::ApplyTempPaintToHitPlayer(APlayerCharacter* OtherPlayer)
{
	FVector TraceBeginLocation = GetOwner()->GetActorLocation();
	FVector TraceEndLocation = OtherPlayer->GetActorLocation();

	FHitResult LineTraceHit;
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(GetOwner());
	TraceParams.bReturnFaceIndex = true;
	GetWorld()->LineTraceSingleByChannel(
		LineTraceHit,
		TraceBeginLocation,
		TraceEndLocation,
		ECC_Visibility,
		TraceParams
	);
	FColor TempPaintColor = Cast<APlayerCharacter>(GetOwner())->GetTempPaintColor();

	//TODO make sure this is set to the correct color in clients
	OtherPlayer->PaintActor(LineTraceHit, TempPaintColor, true);
}

void UForcePushAbilityComponent::ResetAfterCoolDown()
{
	bCanUseAbility = true;
}

void UForcePushAbilityComponent::OnRep_CanUseAbility()
{
	//TODO update UI from this method.
}