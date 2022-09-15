// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "PaintableStaticMeshComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class INKSPLAT_API UPaintableStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UPaintableStaticMeshComponent();

public:
	//returns true if successfuly draws to mesh
	bool PaintMesh(const FHitResult& Hit, const FLinearColor& Color, const float ScaleModifier);

	float CalculatePaintCoverage(const FHitResult& Hit);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UTextureRenderTarget2D* PaintTexture;

	class UMaterial* ParentMaterial;

	UMaterial* BrushMaterial;
	
	UPROPERTY(Category = "Materials", VisibleAnywhere)
	class UMaterialInstanceDynamic* MeshMaterialInstance;

	UPROPERTY(Category = "Materials", VisibleAnywhere)
	UMaterialInstanceDynamic* BrushMaterialInstance;

	class APaintHelper* PaintHelper;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_NumPaintedTiles();

	UFUNCTION()
	void OnRep_LastHitLocation();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Shot Size")
	FVector2D BaseSplatSize = FVector2D(100.f, 100.f);

	UPROPERTY(EditDefaultsOnly, Category = "Painting Settings")
	int32 UVChannelToPaint = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Painting Settings")
	bool bPaintHitLocationOnly = true;

	//TODO move this to the player or weapon class
	float MaterialScaleModifier = 1.0f;

	FVector CalculatePaintScale(FVector Normal);

	TArray<int> PaintCoverageArray;

	UPROPERTY(ReplicatedUsing = OnRep_LastHitLocation)
	FVector2D LastHitLocation;

	UPROPERTY(ReplicatedUsing = OnRep_NumPaintedTiles)
	int32 NumPaintedTiles = 0;

	int32 MaxPaintedTiles = 100;

private:
	void CalculateUVStretchAndScale(const FHitResult& Hit, const FVector2D& UVPosition, float& OutScale, FVector& OutStretch);

	FHitResult ConstructOffsetHitResult(FVector Location, int32 FaceIndex);
};
