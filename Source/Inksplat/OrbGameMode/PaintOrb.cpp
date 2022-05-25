// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintOrb.h"

// Sets default values
APaintOrb::APaintOrb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APaintOrb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APaintOrb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

