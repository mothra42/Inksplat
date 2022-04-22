// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintGun.h"
#include "../Projectiles/InksplatProjectile.h"
#include "../PlayerActor/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

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
	GunMesh->bOnlyOwnerSee = false;

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(GunMesh);
	MuzzleLocation->SetRelativeLocation(FVector(0.0f, 57.0f, 11.0f));
	MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
}

// Called when the game starts or when spawned
void APaintGun::BeginPlay()
{
	Super::BeginPlay();
}

void APaintGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APaintGun, PlayerOwner);
}

// Called every frame
void APaintGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APaintGun::Server_FireProjectile()
{
	//spawn projectile on server
	Server_SpawnProjectile();

	//TODO spawn particle fx
}

void APaintGun::Server_SpawnProjectile()
{
	if (HasAuthority())
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const FRotator SpawnRotation = MuzzleLocation->GetComponentRotation();
			const FVector SpawnLocation = MuzzleLocation->GetComponentLocation();
			//const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			//const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Instigator = PlayerOwner;//GetInstigator();
			SpawnParameters.Owner = PlayerOwner;

			// spawn the projectile at the muzzle
			World->SpawnActor<AInksplatProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParameters);
		}
	}
}

void APaintGun::SetOwningPlayer(APlayerCharacter* NewPlayerOwner)
{
	if (PlayerOwner != NewPlayerOwner)
	{
		SetInstigator(NewPlayerOwner);
		PlayerOwner = NewPlayerOwner;
		// net owner for RPC calls
		SetOwner(NewPlayerOwner);
	}
}

void APaintGun::OnRep_PlayerOwner()
{
	SetOwningPlayer(PlayerOwner);
}

void APaintGun::TriggerRPCTest()
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to trigger rpc"));
	RPC_Test();
}

void APaintGun::RPC_Test_Implementation()
{
	if (PlayerOwner->GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("RPC IS WORKING"))
	}
}

//----TODO clean up code order into sections----
void APaintGun::FireWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to fire weapon"));
	if (bCanFire)
	{
		bCanFire = false;
		UE_LOG(LogTemp, Warning, TEXT("Calling Server handle fire"));
		ServerHandleFire(true);
	}
}

void APaintGun::StopFiringWeapon()
{
		OnFireStopped();
}

void APaintGun::OnFireStopped()
{
	ServerHandleFire(false);
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_FireCooldownPeriod,
		this,
		&APaintGun::ResetAfterCooldown,
		FireCooldownPeriod,
		false
	);
}

void APaintGun::ResetAfterCooldown()
{
	bCanFire = true;
}

void APaintGun::ServerHandleFire_Implementation(bool bShouldFire)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling RPC"));
	UWorld* World = GetWorld();
	if (PlayerOwner->GetLocalRole() == ROLE_Authority)
	{
		if (bShouldFire)
		{
			World->GetTimerManager().SetTimer(
				TimerHandle_TimeBetweenProjectiles,
				this,
				&APaintGun::FireGun,
				TimeBetweenProjectiles,
				true
			);
		}
		else
		{
			World->GetTimerManager().ClearTimer(TimerHandle_TimeBetweenProjectiles);
		}
	}
}

void APaintGun::FireGun()
{
	if (HasAuthority())
	{
		Server_FireProjectile();
	}
}