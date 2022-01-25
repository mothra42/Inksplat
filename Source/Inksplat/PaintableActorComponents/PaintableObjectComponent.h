// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PaintableObjectComponent.generated.h"

class UMaterialInstanceDynamic;
class UTextureRenderTarget2D;
class APaintApplyingCapture;
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
	FPaintInstructions( 
		 FVector InHitLocation,
		 float InBrushRadius,
		 UPrimitiveComponent* InMesh,
		 UMaterialInstanceDynamic* InOriginalMaterial,
		 UMaterialInstanceDynamic* InUnwrapMaterial,
		 UTextureRenderTarget2D* InRenderTarget) 
			:HitLocation(InHitLocation), BrushRadius(InBrushRadius), Mesh(InMesh),
				OriginalMaterial(InOriginalMaterial), UnwrapMaterial(InUnwrapMaterial),
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

	UMaterial* UnwrapParent;

	UPROPERTY(VisibleAnywhere)
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

	/*Looks for queue with the fewest jobs in it and chooses it.
	* If for some reason a queue can't be found it this method returns false
	* */
	bool FindAvailablePaintQueue(const FHitResult& HitResult, const float& BrushRadius);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Enqueue's Paint Instructions based on hit location from projectile
	//TODO revisit this to hold color information too
	void EnqueuePaintInstructions(
		const FVector& InHitLocation, 
		const float& BrushRadius, 
		TQueue<FPaintInstructions>& PaintQueue, 
		APaintApplyingCapture* Capture
	);
	
};
