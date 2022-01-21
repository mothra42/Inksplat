// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PaintableObjectComponent.generated.h"

class UMaterialInstanceDynamic;
class UTextureRenderTarget2D;
class UMaterial;

USTRUCT()
struct FPaintInstructions
{
	GENERATED_BODY()

	FVector HitLocation;
	float BrushRadius;
	UPrimitiveComponent* Mesh;
	UMaterialInstanceDynamic* OriginalMaterial;
	UMaterialInstanceDynamic* UnwrapMaterial;
	UTextureRenderTarget2D* RenderTarget;

	FPaintInstructions() {}
	FPaintInstructions( FVector InHitLocation,
		 float InBrushRadius,
		 UPrimitiveComponent* InMesh,
		 UMaterialInstanceDynamic* InOriginalMaterial,
		 UMaterialInstanceDynamic* InUnwrapMaterial,
		 UTextureRenderTarget2D* InRenderTarget) 
			:HitLocation(InHitLocation), BrushRadius(InBrushRadius), Mesh(InMesh),
				OriginalMaterial(InOriginalMaterial), UnwrapMaterial(UnwrapMaterial),
				RenderTarget(InRenderTarget)
	{
	}
};

/*This class should be attached to any actor that can be painted.
* It is the primary intermediary between the projectile hit that applies paint
* and the scene capture that actually applies the paint.
* this component is notified on hit and sends a message on a queue with all necessary info
* to one of the queue consumers which will be a PaintApplyingCapture.
* */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INKSPLAT_API UPaintableObjectComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPrimitiveComponent* MeshToPaint;
	UTextureRenderTarget2D* PaintMask;
	UMaterialInstanceDynamic* OriginalMaterial;
	UMaterialInstanceDynamic* UnwrapMaterial;

public:	
	// Sets default values for this component's properties
	UPaintableObjectComponent();


	//Responsible for setting initial values and also setting up mesh with proper textures.
	UFUNCTION(BlueprintCallable)
	void SetupPaintableObject(
		UPrimitiveComponent* MeshToSet,
		UMaterial* OriginalMaterialParent
	);

	// Enqueue's Paint Instructions based on hit location from projectile
	//TODO revisit this to hold color information too
	void EnqueuePaintInstructions(FVector HitLocation, float BrushRadius, TQueue<FPaintInstructions> PaintQueue);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
};
