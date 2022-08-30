// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainScanComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../PlayerActor/PlayerCharacter.h"
#include "../PaintableGeometry/PaintableActorBase.h"
#include "Net/UnrealNetwork.h"

UTerrainScanComponent::UTerrainScanComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

void UTerrainScanComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTerrainScanComponent::ServerExecuteAbility_Implementation()
{
	TArray<APaintableActorBase*> ActorsWithinRange = GetScannableActors();
	FVector ScanOrigin = FindScanOrigin();
	ScanActors(ActorsWithinRange, ScanOrigin);
	BeginCoolDown();
}

TArray<APaintableActorBase*> UTerrainScanComponent::GetScannableActors()
{
	TArray<APaintableActorBase*> ActorsWithinRange{};
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		for (TObjectIterator<APaintableActorBase> Itr; Itr; ++Itr)
		{
			ActorsWithinRange.Add(*Itr);
		}
	}

	return ActorsWithinRange;
}

FVector UTerrainScanComponent::FindScanOrigin()
{
	FHitResult Hit;
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), AllPlayers);
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = false;
	TraceParams.AddIgnoredActors(AllPlayers);

	FVector TraceBegin = GetOwner()->GetActorLocation();
	FVector PlayerForwardDirection = GetOwner()->GetActorForwardVector().GetSafeNormal();
	FVector TraceEnd = TraceBegin + (PlayerForwardDirection * 1000);

	bool bHitWorldGeometry = GetWorld()->LineTraceSingleByChannel(
		Hit,
		TraceBegin,
		TraceEnd,
		ECC_Visibility,
		TraceParams
	);

	if (bHitWorldGeometry)
	{
		return Hit.Location;
	}
	
	return TraceEnd;
}

void UTerrainScanComponent::ScanActors_Implementation(const TArray<APaintableActorBase*>& ActorsToScan, const FVector& ScanOrigin)
{
	//FVector PlayerForwardDirection = GetOwner()->GetActorForwardVector().GetSafeNormal();
	//FVector ScanOrigin = GetOwner()->GetActorLocation() + (PlayerForwardDirection * 1000);
	for (APaintableActorBase* ActorToScan : ActorsToScan)
	{
		if (ActorToScan != nullptr)
		{
			ActorToScan->ScanActor(ScanSpeed, ScanRange, ScanOrigin);
		}
	}
}