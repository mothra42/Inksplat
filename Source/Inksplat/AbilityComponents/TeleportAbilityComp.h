// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbilityComponent.h"
#include "TeleportAbilityComp.generated.h"

/**
 * 
 */
UCLASS()
class INKSPLAT_API UTeleportAbilityComp : public UBaseAbilityComponent
{
	GENERATED_BODY()
	
public:
	UTeleportAbilityComp();

protected:
	UPROPERTY(Category = "Teleport Settings", EditDefaultsOnly)
	float TeleportRange = 1000.f;

	UPROPERTY(Category = "Teleport Settings", EditDefaultsOnly)
	float PawnHeightCorrection = 96.f;

	float PawnRadius = 55.f;

	UPROPERTY(Category = "Teleport Attack Settings", EditDefaultsOnly)
	float PaintSprayRadius = 250.f;

	UPROPERTY(Category = "Teleport Attack Settings", EditDefaultsOnly)
	float PaintTextureScaleModifier = 2.5f;

	UPROPERTY(Category = "Teleport Attack Settings", EditDefaultsOnly)
	bool bShowDebugSphere = false;

	UPROPERTY(Category = "Teleport Attack Settings", EditDefaultsOnly)
	int32 NumPitchDivisions = 20;

	UPROPERTY(Category = "Teleport Attack Settings", EditDefaultsOnly)
	int32 NumYawDivisions = 20;

	UPROPERTY(Category = "Teleport Attack Settings", EditDefaultsOnly)
	float PaintSplatterDelayTime = 0.01;

	UPROPERTY(Category = "Teleport Settings", EditDefaultsOnly)
	FVector ZPlaneLineTraceOffset = FVector(0, 0, 20000.f);

	FTimerHandle PaintSplatterDelay;

	FTimerDelegate PaintSplatterDelegate;

private:
	virtual void ServerExecuteAbility_Implementation() override;

	FVector FindTeleportLocation();

	FVector FindSafeTeleportLocation(const FVector TeleportDirection);

	void CorrectTeleportElevation(FVector& XYLocation);

	//utility method used to line trace and find any blocking geometry
	void LineTraceForGeometry(TArray<FHitResult>& OutHitResults, const FVector StartLocation, const FVector EndLocation);

	bool IsLocationInGeometry(const FVector& TeleportLocation);

	void TeleportToLocation(const FVector LocationToTeleportTo);

	UFUNCTION()
	void SplatterPaint(const FVector& SplatterOrigin);

	UFUNCTION(NetMulticast, Reliable)
	void PaintSurface(const FVector& Origin, const TArray<FVector>& EndTraceLocations);


};
