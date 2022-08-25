// Fill out your copyright notice in the Description page of Project Settings.


#define TELEPORT_TRACE ECC_GameTraceChannel2

#include "TeleportAbilityComp.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "../PlayerActor/PlayerCharacter.h"
#include "DrawDebugHelpers.h"

//for splatter paint
#include "../Interfaces/PaintableObjectInterface.h"

UTeleportAbilityComp::UTeleportAbilityComp()
{
	SetIsReplicated(true);
}

void UTeleportAbilityComp::ServerExecuteAbility_Implementation()
{
	FVector TeleportLocation = FindTeleportLocation();
	TeleportToLocation(TeleportLocation);
	SplatterPaint(TeleportLocation, 10, 10);
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

void UTeleportAbilityComp::SplatterPaint(const FVector& SplatterOrigin, const int32 NumLatitudeSegments, const int32 NumLongitudeSegments)
{
	TArray<FVector> EndLocations;
	for (int i = 0; i <= NumLongitudeSegments; i++)
	{
		float Pitch = 90 - ((180 / NumLongitudeSegments) * i);
		for (int j = 0; j <= NumLatitudeSegments; j++)
		{
			float Yaw = 0 - ((360 / NumLatitudeSegments) * j);
			FVector EndLocation = FRotator(Pitch, Yaw, 0).Vector() * PaintSprayRadius + SplatterOrigin;
			EndLocations.Add(EndLocation);
			DrawDebugLine(GetWorld(),
				SplatterOrigin,
				EndLocation,
				FColor::Purple,
				false,
				10.f);
		}
	 }

	PaintSurface(SplatterOrigin, EndLocations);
}

void UTeleportAbilityComp::PaintSurface_Implementation(const FVector& Origin, const TArray<FVector>& EndTraceLocations)
{

	FHitResult LineTraceHit;
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(GetOwner());
	TraceParams.bReturnFaceIndex = true;

	for (FVector EndTraceLocation : EndTraceLocations)
	{
		bool bTraceHit = GetWorld()->LineTraceSingleByChannel(
			LineTraceHit,
			Origin,
			EndTraceLocation,
			ECC_Camera,
			TraceParams
		);

		if (bTraceHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor to paint is %s"), *LineTraceHit.Actor->GetName());
			IPaintableObjectInterface* PaintableObject = Cast<IPaintableObjectInterface>(LineTraceHit.Actor);
			if (PaintableObject)
			{
				APlayerCharacter* OwningPawn = Cast<APlayerCharacter>(GetOwner());
				//TODO get owning actors color here.
				PaintableObject->PaintActor(LineTraceHit, OwningPawn->GetPaintColor());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Paintable Object is null"));
			}
		}
	}
}