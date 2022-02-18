// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "PaintableSkeletalMeshComponent.generated.h"

USTRUCT()
struct FHealthCheckInstructions
{
	GENERATED_BODY()

	FVector2D Origin;
	int32 UBounds;
	int32 VBounds;

	FHealthCheckInstructions() {}
	FHealthCheckInstructions(
		FVector2D InOrigin,
		int32 InUBounds,
		int32 InVBounds)
		:Origin(InOrigin), UBounds(InUBounds), VBounds(InVBounds) {}
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
	bool PaintMesh(const FHitResult& Hit, const FLinearColor& Color);

protected:
	class UTextureRenderTarget2D* PaintTexture;

	class UMaterial* ParentMaterial;

	class UMaterialInstanceDynamic* MeshMaterialInstance;

private:
	TArray<int> PaintCoverageArray;

	TQueue<FHealthCheckInstructions> HealthCheckQueue;

	bool bIsConsumingHealthCheck = false;

protected:
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void ConsumeHealthCheckQueue();

	float CalculateHealth(FVector2D Origin, int32 U_Bounds, int32 V_Bounds);
};
