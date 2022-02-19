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
	bool PaintMesh(const FHitResult& Hit, const FLinearColor& Color);

protected:
	class UTextureRenderTarget2D* PaintTexture;

	class UMaterial* ParentMaterial;

	class UMaterialInstanceDynamic* MeshMaterialInstance;

private:
	TArray<int> PaintCoverageArray;
	TArray<int> TestArray;
	int32 NumPaintedTiles = 0;

protected:
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	int32 CalculateHealth(FVector2D Origin);
};
