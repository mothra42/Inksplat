// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableObjectComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/Material.h"
#include "Kismet/KismetRenderingLibrary.h"

// Sets default values for this component's properties
UPaintableObjectComponent::UPaintableObjectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	PaintMask = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 1024, 1024);

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

void UPaintableObjectComponent::EnqueuePaintInstructions(FVector HitLocation, float BrushRadius, TQueue<FPaintInstructions> PaintQueue)
{
	//TODO Construct Paint instructions based on parameters and owner information.
	//FPaintInstructions PaintInstructions(HitLocation, BrushRadius, MeshToPaint, MeshToPaint->GetMaterial(0), UnwrapMaterial, PaintMask);

	//Add FPaintInstructions struct to queue.

	//TODO figure out where the queue consumer lives, does each PaintApplyingCapture have a queue?
	//Or is there one long queue and a single consumer that distributes the instructions to each PaintApplyingCapture.
}
