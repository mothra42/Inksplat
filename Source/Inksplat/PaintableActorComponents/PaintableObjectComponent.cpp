// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableObjectComponent.h"
#include "../PaintApplicationFactory/PaintApplyingCapture.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/Material.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UPaintableObjectComponent::UPaintableObjectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	

	static ConstructorHelpers::FObjectFinder<UMaterial> UnwrapMaterialObjectFinder(
		TEXT("/Game/PaintApplicationFactory/Materials/M_Unwrap")
	);
	UMaterial* UnwrapParent = UnwrapMaterialObjectFinder.Object;
	UnwrapMaterial = UMaterialInstanceDynamic::Create(UnwrapParent, NULL);
}


// Called when the game starts
void UPaintableObjectComponent::BeginPlay()
{
	Super::BeginPlay();
	PaintMask = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 1024, 1024);
}

void UPaintableObjectComponent::SetupPaintableObject(
	UPrimitiveComponent* MeshToSet,
	UMaterial* OriginalMaterialParent)
{
	MeshToPaint = MeshToSet;
	OriginalMaterial = UMaterialInstanceDynamic::Create(OriginalMaterialParent, MeshToPaint);
	OriginalMaterial->SetTextureParameterValue(FName("OpacityMask"), PaintMask);
	MeshToPaint->SetMaterial(0, OriginalMaterial);
}

bool UPaintableObjectComponent::FindAvailablePaintQueue(const FHitResult& HitResult, const float& BrushRadius)
{
	TArray<AActor*> CaptureArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APaintApplyingCapture::StaticClass(), CaptureArray);
	if (CaptureArray.Num() > 0)
	{
		APaintApplyingCapture* ChosenCapture = Cast<APaintApplyingCapture>(CaptureArray[0]);
		for (AActor* Actor : CaptureArray)
		{
			APaintApplyingCapture* CurrentCapture = Cast<APaintApplyingCapture>(Actor);
			if (ChosenCapture != nullptr && CurrentCapture != nullptr)
			{
				if (CurrentCapture->GetNumOfJobsInQueue() < ChosenCapture->GetNumOfJobsInQueue())
				{
					ChosenCapture = CurrentCapture;
				}
			}
			else if (CurrentCapture != nullptr)
			{
				ChosenCapture = CurrentCapture;
			}
			else
			{
				continue;
			}
		}

		if (ChosenCapture != nullptr)
		{
			EnqueuePaintInstructions(HitResult.Location, BrushRadius, ChosenCapture->GetPaintingQueue(), ChosenCapture);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void UPaintableObjectComponent::EnqueuePaintInstructions(
	const FVector& InHitLocation, 
	const float& BrushRadius, 
	TQueue<FPaintInstructions>& PaintQueue,
	APaintApplyingCapture* Capture
)
{
	if (UnwrapMaterial != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnwrapMaterial is null in class"));
	}

	//Add FPaintInstructions struct to queue.
	FPaintInstructions PaintInstructions = FPaintInstructions(
		InHitLocation, 
		BrushRadius, 
		MeshToPaint, 
		OriginalMaterial, 
		UnwrapMaterial, 
		PaintMask
	);

	PaintQueue.Enqueue(PaintInstructions);
	Capture->IncrementJobs();
	Capture->BeginConsumingPaintingJobs();
	//TODO figure out where the queue consumer lives, does each PaintApplyingCapture have a queue?
	//Or is there one long queue and a single consumer that distributes the instructions to each PaintApplyingCapture.
}
