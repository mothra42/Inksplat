// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaintGun.generated.h"

UCLASS()
class INKSPLAT_API APaintGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APaintGun();

protected:
	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* GunMesh;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* MuzzleLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

//Equiping/Setting up owner methods and variables
protected:
	//OwningPawn
	UPROPERTY(Transient, ReplicatedUsing = OnRep_PlayerOwner)
	class APlayerCharacter* PlayerOwner;

public:
	void SetOwningPlayer(APlayerCharacter* NewPlayerOwner);

	UFUNCTION()
	void OnRep_PlayerOwner();

//Weapon Paint Properties and Methods
private:
	FColor PaintColor;

public:
	void SetPaintColor(FColor ColorToSet);

//Weapon Firing methods and timer handles
protected:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AInksplatProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Gun Properties")
	float FireRate = 0.1f;
	FTimerHandle TimerHandle_TimeBetweenProjectiles;

	UPROPERTY(EditDefaultsOnly, Category = "Gun Properties")
	float FireCooldownPeriod = 0.25f;
	FTimerHandle TimerHandle_FireCooldownPeriod;

public:
	void FireWeapon();

	void StopFiringWeapon();

protected:
	void ResetAfterCooldown();

	/** Server function for spawning projectiles.*/
	UFUNCTION(Server, Reliable)
	void ServerHandleFire(bool bShouldFire);

	void Server_SpawnProjectile();

	bool bCanFire = true;

};
