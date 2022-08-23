// Fill out your copyright notice in the Description page of Project Settings.


#define TELEPORT_TRACE ECC_GameTraceChannel2

#include "TeleportAbilityComp.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "../PlayerActor/PlayerCharacter.h"
#include "DrawDebugHelpers.h"

UTeleportAbilityComp::UTeleportAbilityComp()
{
	SetIsReplicated(true);
}

void UTeleportAbilityComp::ServerExecuteAbility_Implementation()
{
	TeleportToLocation(FindTeleportLocation());
}

FVector UTeleportAbilityComp::FindTeleportLocation()
{
	FVector TeleportLocation;
	FVector PlayerForwardDirection = GetOwner()->GetActorForwardVector();
	FVector PlayerTeleportDirection = FVector(PlayerForwardDirection.X, PlayerForwardDirection.Y, 0).GetSafeNormal();
	TeleportLocation = FindSafeTeleportLocation(PlayerTeleportDirection);
	return TeleportLocation;
}

FVector UTeleportAbilityComp::FindSafeTeleportLocation(const FVector TeleportDirection)
{
	//Set initial teleportation location before teleport safety checks.
	FVector TeleportLocation = GetOwner()->GetActorLocation() + (TeleportDirection * TeleportRange);

	TArray<FHitResult> HitResults;
	LineTraceForGeometry(HitResults,
		GetOwner()->GetActorLocation(),
		TeleportLocation
	);

	for (FHitResult HitResult : HitResults)
	{
		for (FName Tag : HitResult.Actor->Tags)
		{
			//If the teleport goes past level extents
			if (Tag == FName("LevelExtent"))
			{
				//TODO remove magic number 55.f for the radius of owner's capsule component
				TeleportLocation = HitResult.Location - (TeleportDirection * 55.f);
			}
		}
	}

	CorrectTeleportElevation(TeleportLocation);

	return TeleportLocation;
}

void UTeleportAbilityComp::CorrectTeleportElevation(FVector& XYLocation)
{
	//This method will line trace down towards the 0 plane to find the closest z plane to the player and place them there.
	//this is used to handle ramps or any uneven geometry.
	TArray<FHitResult> HitResults;
	LineTraceForGeometry(HitResults,
		ZPlaneLineTraceOffset + XYLocation,
		FVector(XYLocation.X, XYLocation.Y, -10)
	);
	//Find Hit location that is closest to player's current elevation
	XYLocation = HitResults.Num() >= 1 ? HitResults[0].Location : XYLocation;
	float ClosestElevation = FMath::Abs(GetOwner()->GetActorLocation().Z - XYLocation.Z);
	for (FHitResult HitResult : HitResults)
	{
		float PotentialClosestElevation = FMath::Abs(GetOwner()->GetActorLocation().Z - HitResult.Location.Z);
		if (PotentialClosestElevation <= ClosestElevation)
		{
			XYLocation.Z = HitResult.Location.Z;
			ClosestElevation = PotentialClosestElevation;
		}
	}

	XYLocation.Z += 96.f;
}

void UTeleportAbilityComp::LineTraceForGeometry(TArray<FHitResult>& OutHitResults, const FVector StartLocation, const FVector EndLocation)
{
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), AllPlayers);
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = false;
	TraceParams.AddIgnoredActors(AllPlayers);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 10.f, 0, 3.f);
	GetWorld()->LineTraceMultiByChannel(OutHitResults,
		StartLocation,
		EndLocation,
		TELEPORT_TRACE,
		TraceParams);
}

bool UTeleportAbilityComp::IsLocationInGeometry(const FVector& TeleportLocation)
{
	//FCollisionShape Sphere = FCollisionShape::MakeSphere(100.f);
	//UE_LOG(LogTemp, Warning, TEXT("Testing if location is in geometry"));
	//DrawDebugSphere(GetWorld(), TeleportLocation, 100, 8, FColor::Green, false, 10.f);
	//TArray<FHitResult> Hits;
	//TArray<AActor*> AllPlayers;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), AllPlayers);
	//FCollisionQueryParams TraceParams;
	//TraceParams.bTraceComplex = false;
	//TraceParams.AddIgnoredActors(AllPlayers);
	//bool bIsOverlapped =  GetWorld()->SweepMultiByChannel(Hits,
	//	TeleportLocation,
	//	TeleportLocation + FVector(0, 0, 1.0),
	//	GetOwner()->GetActorRotation().Quaternion(),
	//	ECC_Visibility,
	//	Sphere,
	//	TraceParams
	//);
	//UE_LOG(LogTemp, Warning, TEXT("Found overlapping geometry %i"), bIsOverlapped);
	return false;
}

void UTeleportAbilityComp::TeleportToLocation(const FVector LocationToTeleportTo)
{
	GetOwner()->TeleportTo(LocationToTeleportTo,
		GetOwner()->GetActorRotation(), 
		false, 
		true
	);
}

void UTeleportAbilityComp::SplatterPaint(const FVector& TeleportLocation)
{
	//Do a sphere trace looking for paintable actors
		//for each paintable actor found 
			//paint the point closest to the teleport location
			//find a number(6 to start with) of other points to paint to make a splatter like effect.
				//paint each of those points.
}