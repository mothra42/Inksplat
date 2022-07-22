// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "PaintableSkeletalMeshComponent.generated.h"

UENUM()
enum class ETempPaintApplicationType : uint8
{
	Add,
	Remove,
	Static
};

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

	class UMaterial* ParentMaterial;

	UMaterial* BrushMaterial;

	UPROPERTY(Category = "Materials", BlueprintReadWrite, VisibleAnywhere)
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

	ETempPaintApplicationType TempPaintApplicationType = ETempPaintApplicationType::Static;

	UPROPERTY(ReplicatedUsing=OnRep_TempPaintCoverageFactor)
	float TempPaintCoverageFactor = 0.0;

	UPROPERTY(ReplicatedUsing = OnRep_TempPaintColor)
	FColor TempPaintColor;

	//TODO make a better name
	UPROPERTY(Category = "Temporary Paint", EditDefaultsOnly)
	float TempPaintCoverageTickAddition = 0.01;

	UPROPERTY(Category = "Temporary Paint", EditDefaultsOnly)
	float TempPaintCoverageTickSubtraction = 0.001;

protected:
	UFUNCTION()
	void OnRep_NumPaintedTiles();

	UFUNCTION()
	void OnRep_TempPaintCoverageFactor();

	UFUNCTION()
	void OnRep_TempPaintColor();

	void UpdateParentHealth();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void CalculateUVStretchAndScale(const FHitResult& Hit, const FVector2D& UVPosition, float& OutScale, FVector& OutStretch);

	FHitResult ConstructOffsetHitResult(FVector Location, int32 FaceIndex, FName BoneName);

	float CalculatePaintCoverage(FVector2D Origin);

	void UpdateTempPaintCoverageFactor();

public:
	void SetMaxPaintedTiles(int32 MaxTilesToSet) { MaxPaintedTiles = MaxTilesToSet; }
};
