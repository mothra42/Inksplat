// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintOrb.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APaintOrb::APaintOrb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OrbMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbMesh"));
	OrbMesh->SetupAttachment(RootComponent);

	bReplicates = true;
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

void APaintOrb::Activate()
{
	UE_LOG(LogTemp, Warning, TEXT("Orb is activated"));
}

//Things an orb must do

// It must be picked up, and maybe even have the ability to be exchanged between team members, a pass if you will
// It must have a timer that describes how long from its activation until color explosion.
// the ball will have two states of activation, it lies dormant until a certain amount of it is revealed through painting
// at that point it can be picked up by any player
// upon picking it up the ball reaches full activation and begins the timer and painting process.

// Probably best if the orb communicates with an actor component for painting, that way
// the orb is more flexible in what it can react to, in case other game modes would use this in the future.
