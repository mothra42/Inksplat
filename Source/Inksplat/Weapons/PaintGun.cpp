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
	DOREPLIFETIME(APaintGun, PaintColor);
}

// Called every frame
void APaintGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//Methods related to equipping weapon
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

//Methods related to painting properties
void APaintGun::SetPaintColor(FColor ColorToSet)
{
	if (PlayerOwner->GetLocalRole() == ROLE_Authority)
	{
		PaintColor = ColorToSet;
	}
}

void APaintGun::OnRep_PaintColor()
{
	if (PlayerOwner->GetLocalRole() < ROLE_Authority)
	{
		SetPaintColor(PaintColor);
	}
}

//Methods related to firing weapon
void APaintGun::FireWeapon()
{
	if (bCanFire)
	{
		//UE_LOG
		bCanFire = false;
		ServerHandleFire(true);
	}

	//// try and play the sound if specified
	//if (FireSound != nullptr)
	//{
	//	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	//}
	//
	//// try and play a firing animation if specified
	//if (FireAnimation != nullptr)
	//{
	//	// Get the animation object for the arms mesh
	//	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	//	if (AnimInstance != nullptr)
	//	{
	//		AnimInstance->Montage_Play(FireAnimation, 1.f);
	//	}
	//}
}

void APaintGun::StopFiringWeapon()
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
	UWorld* World = GetWorld();
	if (PlayerOwner->GetLocalRole() == ROLE_Authority)
	{
		if (bShouldFire)
		{
			World->GetTimerManager().SetTimer(
				TimerHandle_TimeBetweenProjectiles,
				this,
				&APaintGun::Server_SpawnProjectile,
				FireRate,
				true
			);
		}
		else
		{
			World->GetTimerManager().ClearTimer(TimerHandle_TimeBetweenProjectiles);
		}
	}
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