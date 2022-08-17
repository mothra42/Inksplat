// Fill out your copyright notice in the Description page of Project Settings.


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
	//FindTeleportLocation();
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
	float ClosestElevation = FMath::Abs(GetOwner()->GetActorLocation().Z - XYLocation.Z);
	for (FHitResult HitResult : HitResults)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Results is %i long"), HitResults.Num());
		float PotentialClosestElevation = FMath::Abs(GetOwner()->GetActorLocation().Z - HitResult.Location.Z);
		if (!IsLocationInGeometry(HitResult.Location + FVector(0, 0, 96.f)))
		{
			if (PotentialClosestElevation <= ClosestElevation)
			{
				XYLocation.Z = HitResult.Location.Z;
				ClosestElevation = PotentialClosestElevation;
			}
		}
		DrawDebugSphere(GetWorld(), HitResult.Location, 10, 8, FColor::Blue, false, 10.f);
	}

	XYLocation.Z += 96.f;
}

void UTeleportAbilityComp::LineTraceForGeometry(TArray<FHitResult>& OutHitResults, const FVector StartLocation, const FVector EndLocation)
{
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), AllPlayers);
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActors(AllPlayers);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 10.f, 0, 3.f);
	GetWorld()->LineTraceMultiByChannel(OutHitResults,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		TraceParams);
}

bool UTeleportAbilityComp::IsLocationInGeometry(const FVector& TeleportLocation)
{
	FCollisionShape Sphere = FCollisionShape::MakeSphere(10.f);
	UE_LOG(LogTemp, Warning, TEXT("Testing if location is in geometry"));
	DrawDebugSphere(GetWorld(), TeleportLocation, 10, 8, FColor::Green, false, 10.f);
	FHitResult Hit;
	return GetWorld()->SweepSingleByChannel(Hit,
		TeleportLocation,
		TeleportLocation + FVector(0, 0, 0.1),
		GetOwner()->GetActorRotation().Quaternion(),
		ECC_Visibility,
		Sphere
	);
}

void UTeleportAbilityComp::TeleportToLocation(const FVector LocationToTeleportTo)
{
	GetOwner()->TeleportTo(LocationToTeleportTo,
		GetOwner()->GetActorRotation(), 
		false, 
		true
	);
}