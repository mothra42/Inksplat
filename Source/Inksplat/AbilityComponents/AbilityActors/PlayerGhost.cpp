// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGhost.h"

// Sets default values
APlayerGhost::APlayerGhost()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlayerGhost::BeginPlay()
{
	Super::BeginPlay();

}

