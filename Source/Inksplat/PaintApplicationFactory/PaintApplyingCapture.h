// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaintApplyingCapture.generated.h"

class USceneCaptureComponent2D;
class USceneComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UTextureRenderTarget2D;

UCLASS()
class INKSPLAT_API APaintApplyingCapture : public AActor
{
	GENERATED_BODY()

	USceneComponent* SceneRoot;

	UPROPERTY(EditDefaultsOnly, Category = SceneCapture)
	USceneCaptureComponent2D* CaptureComponent;

	UPROPERTY(EditDefaultsOnly, Category = BackgroundPlane)
	UStaticMeshComponent* BackgroundPlane;

public:	
	// Sets default values for this actor's properties
	APaintApplyingCapture();

	void PaintActor(
		FVector HitLocation, 
		float BrushRadius, 
		UPrimitiveComponent* Mesh,
		UMaterialInstanceDynamic* OriginalMaterial,
		UMaterialInstanceDynamic* UnwrapMaterial,
		UTextureRenderTarget2D* RenderTarget);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
