// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "ScannableStaticMesh.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class INKSPLAT_API UScannableStaticMesh : public UStaticMeshComponent
{
	GENERATED_BODY()
public:
	UScannableStaticMesh();

	void ScanMesh(const float ScanSpeed, const float RangeToSet, const FVector& ScanOrigin);

	void ProgressScan();

protected:
	virtual void BeginPlay() override;

	class UMaterial* ParentMaterial;

	UPROPERTY(Category = "Materials", VisibleAnywhere)
	class UMaterialInstanceDynamic* MeshMaterialInstance;

	float MaxRange = 3.0f;

	float CurrentRange = 0.f;

private:
	FTimerHandle TimerHandle_ProgressScan;

	UPROPERTY(EditDefaultsOnly, Category = "Scan Properties")
	float TimeBetweenScanUpdates = 0.01f;
};
