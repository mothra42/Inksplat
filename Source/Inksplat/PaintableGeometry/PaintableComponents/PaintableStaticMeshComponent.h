// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "PaintableStaticMeshComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class INKSPLAT_API UPaintableStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UPaintableStaticMeshComponent();

public:
	//returns true if successfuly draws to mesh
	bool PaintMesh(const FHitResult& Hit, const FLinearColor& Color);

	void ScanMesh(float ScanSpeedToSet);

protected:
	
	UPROPERTY(VisibleAnywhere)
	class UTextureRenderTarget2D* PaintTexture;

	class UMaterial* ParentMaterial;

	UMaterial* BrushMaterial;
	
	UPROPERTY(Category = "Materials", VisibleAnywhere)
	class UMaterialInstanceDynamic* MeshMaterialInstance;

	UPROPERTY(Category = "Materials", VisibleAnywhere)
	UMaterialInstanceDynamic* BrushMaterialInstance;

	UPROPERTY(ReplicatedUsing = OnRep_UpdateSpeedParam)
	float ScanSpeed = 0.0f;

	float MaxRange = 3.0f;

	float CurrentRange = 0.f;

	class APaintHelper* PaintHelper;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_UpdateSpeedParam();

	void StopScan();

	void ProgressScan();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Shot Size")
	FVector2D BaseSplatSize = FVector2D(100.f, 100.f);

	UPROPERTY(EditDefaultsOnly, Category = "Painting Settings")
	int32 UVChannelToPaint = 0;

	FVector CalculatePaintScale(FVector Normal);

	FTimerHandle TimerHandle_ScanTime;

	UPROPERTY(EditDefaultsOnly, Category = "Object Scan")
	float ScanTime = 0.01f;

private:
	void CalculateUVStretchAndScale(const FHitResult& Hit, const FVector2D& UVPosition, float& OutScale, FVector& OutStretch);

	FHitResult ConstructOffsetHitResult(FVector Location, int32 FaceIndex);
};
