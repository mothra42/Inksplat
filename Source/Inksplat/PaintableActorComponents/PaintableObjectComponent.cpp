// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableObjectComponent.h"
#include "Engine/TextureRenderTarget2D.h"

// Sets default values for this component's properties
UPaintableObjectComponent::UPaintableObjectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	PaintMask = NewObject<UTextureRenderTarget2D>();

	//TODO may need to construct this differently based on uv map of actor's mesh
	PaintMask->SizeX = 1024;
	PaintMask->SizeY = 1024;
}


// Called when the game starts
void UPaintableObjectComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UPaintableObjectComponent::EnqueuePaintInstructions(FVector Hitlocation, float BrushRadius, TQueue<FPaintInstructions> PaintQueue)
{
	//TODO Construct Paint instructions based on parameters and owner information.
	//FPaintInstructions PaintInstructions(HitLocation, BrushRadius, Ge)

	//Add FPaintInstructions struct to queue.

	//TODO figure out where the queue consumer lives, does each PaintApplyingCapture have a queue?
	//Or is there one long queue and a single consumer that distributes the instructions to each PaintApplyingCapture.
}
