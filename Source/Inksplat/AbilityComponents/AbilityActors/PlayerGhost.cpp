// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityComponents/AbilityActors/PlayerGhost.h"

// Sets default values
APlayerGhost::APlayerGhost()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerGhost::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

