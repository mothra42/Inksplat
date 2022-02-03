// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "PaintableStaticMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class INKSPLAT_API UPaintableStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UPaintableStaticMeshComponent();

public:
	class UTextureRenderTarget2D* PaintTexture;

protected:
	class UMaterial* ParentMaterial;

	class UMaterialInstanceDynamic* MeshMaterialInstance;

protected:
	virtual void BeginPlay() override;
};
