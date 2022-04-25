// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintHelper.h"

// Sets default values
APaintHelper::APaintHelper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APaintHelper::BeginPlay()
{
	Super::BeginPlay();
	
}

UTexture* APaintHelper::GetPaintSplatTexture()
{
	int32 RandomIndex = FMath::RandRange(0, PaintSplatTextures.Num() - 1);
	return PaintSplatTextures[RandomIndex];
}

