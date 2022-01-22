// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintApplyingCapture.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
APaintApplyingCapture::APaintApplyingCapture()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup Scene Root
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	// Setup Capture Component
	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent"));
	CaptureComponent->SetupAttachment(RootComponent);
	CaptureComponent->bCaptureEveryFrame = false;
	CaptureComponent->bCaptureOnMovement = false;

	// Setup Background Plane
	BackgroundPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackgroundPlane"));
	BackgroundPlane->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void APaintApplyingCapture::BeginPlay()
{
	Super::BeginPlay();
	
}

void APaintApplyingCapture::BeginConsumingPaintingJobs()
{
	if (!bIsConsuming)
	{
		ConsumePaintingJobs();
	}
}

void APaintApplyingCapture::ConsumePaintingJobs()
{
	bIsConsuming = true;
	FPaintInstructions PaintInstructions;

	while (!PaintingQueue.IsEmpty())
	{
		PaintingQueue.Dequeue(PaintInstructions);
		DecrementJobs();
		PaintActor(
			PaintInstructions.HitLocation,
			PaintInstructions.BrushRadius,
			PaintInstructions.Mesh,
			PaintInstructions.OriginalMaterial,
			PaintInstructions.UnwrapMaterial,
			PaintInstructions.RenderTarget
		);
	}

	bIsConsuming = false;
}

void APaintApplyingCapture::PaintActor(FVector HitLocation,
	float BrushRadius,
	UPrimitiveComponent* Mesh,
	UMaterialInstanceDynamic* OriginalMaterial,
	UMaterialInstanceDynamic* UnwrapMaterial,
	UTextureRenderTarget2D* RenderTarget)
{
	//Set Render Target
	CaptureComponent->TextureTarget = RenderTarget;

	//Setup UnwrapMaterial
	if (UnwrapMaterial != nullptr)
	{
		UnwrapMaterial->SetVectorParameterValue(FName("UnwrapLocation"), FLinearColor(GetActorLocation()));
		UnwrapMaterial->SetVectorParameterValue(FName("HitLocation"), FLinearColor(HitLocation));
		UnwrapMaterial->SetScalarParameterValue(FName("CaptureSize"), CaptureComponent->OrthoWidth);
		UnwrapMaterial->SetScalarParameterValue(FName("BrushRadius"), BrushRadius);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unwrap Material is null"));
	}

	//Capture UnwrappedMaterial
	Mesh->SetMaterial(0, UnwrapMaterial);
	CaptureComponent->CaptureScene();
	Mesh->SetMaterial(0, OriginalMaterial);
}

TQueue<FPaintInstructions>& APaintApplyingCapture::GetPaintingQueue()
{
	return PaintingQueue;
}

int32 APaintApplyingCapture::GetNumOfJobsInQueue()
{
	return NumJobsInQueue;
}
