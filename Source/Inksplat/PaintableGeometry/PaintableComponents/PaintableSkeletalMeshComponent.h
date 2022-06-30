// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "PaintableSkeletalMeshComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class INKSPLAT_API UPaintableSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UPaintableSkeletalMeshComponent();

public:
	//returns true if successfuly draws to mesh
	UFUNCTION(BlueprintCallable)
	bool PaintMesh(const FHitResult& Hit, const FLinearColor& Color, bool bIsTemporary = false, float TempPaintLifetime = 0.f);

protected:
	class UTextureRenderTarget2D* PaintTexture;

	UTextureRenderTarget2D* TempPaintTexture;

	class UMaterial* ParentMaterial;

	UMaterial* BrushMaterial;

	class UMaterialInstanceDynamic* MeshMaterialInstance;

	UPROPERTY(Category = "Materials", VisibleAnywhere)
	UMaterialInstanceDynamic* BrushMaterialInstance;

	UPROPERTY(Category = "Texture Correction", EditDefaultsOnly)
	float MaterialScale = 0.2;

	UPROPERTY(Category = "Texture Correction", EditDefaultsOnly)
	FVector MaterialStretch = FVector(1.0, 1.0, 0.0);

	class APaintHelper* PaintHelper;

private:
	TArray<int> PaintCoverageArray;

	UPROPERTY(ReplicatedUsing=OnRep_NumPaintedTiles)
	int32 NumPaintedTiles = 0;

	int32 MaxPaintedTiles = 100;
protected:
	UFUNCTION()
	void OnRep_NumPaintedTiles();

	void UpdateParentHealth();

protected:
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void CalculateUVStretchAndScale(const FHitResult& Hit, const FVector2D& UVPosition, float& OutScale, FVector& OutStretch);

	FHitResult ConstructOffsetHitResult(FVector Location, int32 FaceIndex, FName BoneName);

	float CalculatePaintCoverage(FVector2D Origin);

	void RemoveTemporaryPaint();

public:
	void SetMaxPaintedTiles(int32 MaxTilesToSet) { MaxPaintedTiles = MaxTilesToSet; }
};
