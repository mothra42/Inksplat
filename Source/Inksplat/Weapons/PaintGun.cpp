// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintGun.h"
#include "../Projectiles/InksplatProjectile.h"

// Sets default values
APaintGun::APaintGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	RootComponent = GunMesh;
	GunMesh->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;
	GunMesh->bOnlyOwnerSee = true;

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(GunMesh);
	MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
}

// Called when the game starts or when spawned
void APaintGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APaintGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APaintGun::Server_FireProjectile()
{

}

