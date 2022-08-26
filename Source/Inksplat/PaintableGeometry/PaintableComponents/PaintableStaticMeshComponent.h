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
	bool PaintMesh(const FHitResult& Hit, const FLinearColor& Color, const float ScaleModifier);

protected:
	
	UPROPERTY(VisibleAnywhere)
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

private:
	UPROPERTY(EditDefaultsOnly, Category = "Shot Size")
	FVector2D BaseSplatSize = FVector2D(100.f, 100.f);

	UPROPERTY(EditDefaultsOnly, Category = "Painting Settings")
	int32 UVChannelToPaint = 0;

	FVector CalculatePaintScale(FVector Normal);

private:
	void CalculateUVStretchAndScale(const FHitResult& Hit, const FVector2D& UVPosition, float& OutScale, FVector& OutStretch);

	FHitResult ConstructOffsetHitResult(FVector Location, int32 FaceIndex);
};
