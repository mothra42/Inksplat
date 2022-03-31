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

protected:
	
	class UTextureRenderTarget2D* PaintTexture;

	class UMaterial* ParentMaterial;

	class UMaterialInstanceDynamic* MeshMaterialInstance;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Shot Size")
	FVector2D BaseSplatSize = FVector2D(100.f, 100.f);

	FVector2D CalculatePaintScale(FVector Normal);
};
