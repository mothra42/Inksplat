// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleportAbilityComp.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

UTeleportAbilityComp::UTeleportAbilityComp()
{
	SetIsReplicated(true);
}

void UTeleportAbilityComp::ServerExecuteAbility_Implementation()
{
	//TeleportToLocation(FindTeleportLocation());
	FindTeleportLocation();
}

FVector UTeleportAbilityComp::FindTeleportLocation()
{
	FVector TeleportLocation;
	FVector PlayerForwardDirection = GetOwner()->GetActorForwardVector();
	FVector PlayerTeleportDirection = FVector(PlayerForwardDirection.X, PlayerForwardDirection.Y, 0).GetSafeNormal();
	FindSafeTeleportLocation(PlayerTeleportDirection);
	////Using Forward vector is slightly fraut because it could be pointing up or down.
	//TeleportLocation = GetOwner()->GetActorLocation() + 
	//	(PlayerTeleportDirection * TeleportRange);
	return TeleportLocation;
}

FVector UTeleportAbilityComp::FindSafeTeleportLocation(const FVector TeleportDirection)
{
	TArray<FHitResult> HitResults;

	LineTraceForGeometry(HitResults,
		GetOwner()->GetActorLocation(),
		GetOwner()->GetActorLocation() + (TeleportDirection * TeleportRange)
	);

	for (FHitResult HitResult : HitResults)
	{
		for (FName Tag : HitResult.Actor->Tags)
		{
			//If the teleport goes past level extents
			if (Tag == FName("LevelExtent"))
			{
				//TODO remove magic number 55.f for the radius of owner's capsule component
				FVector AdjustedLocation = HitResult.Location - (TeleportDirection * 55.f);
				return FindCorrectZPlacement(AdjustedLocation);
			}
		}
	}
	return FindCorrectZPlacement(GetOwner()->GetActorLocation() + (TeleportDirection * TeleportRange));
}

FVector UTeleportAbilityComp::FindCorrectZPlacement(FVector XYLocation)
{
	//This method will line trace down towards the 0 plane to find the closest z plane to the player and place them there.
	//this is used to handle ramps or any uneven geometry.
	return FVector{};
}

void UTeleportAbilityComp::LineTraceForGeometry(TArray<FHitResult>& OutHitResults, const FVector StartLocation, const FVector EndLocation)
{
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(GetOwner());
	GetWorld()->LineTraceMultiByChannel(OutHitResults,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		TraceParams);
}

void UTeleportAbilityComp::TeleportToLocation(const FVector LocationToTeleportTo)
{
	GetOwner()->TeleportTo(LocationToTeleportTo,
		GetOwner()->GetActorRotation(), 
		false, 
		true
	);
}