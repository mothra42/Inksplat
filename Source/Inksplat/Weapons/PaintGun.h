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

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AInksplatProjectile> ProjectileClass;

protected:
	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* GunMesh;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* MuzzleLocation;

	//OwningPawn
	UPROPERTY(Transient, ReplicatedUsing = OnRep_PlayerOwner)
	class APlayerCharacter* PlayerOwner;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Server_SpawnProjectile();

	UFUNCTION()
	void OnRep_PlayerOwner();

	UFUNCTION(Server, Reliable)
	void RPC_Test();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Server_FireProjectile();

	void TriggerRPCTest();

	void SetOwningPlayer(APlayerCharacter* NewPlayerOwner);

//moved over logic TODO clean up into sections
public:
	void FireWeapon();

	void StopFiringWeapon();

protected:
	
	void OnFire();

	void OnFireStopped();

	void ResetAfterCooldown();

	/** Server function for spawning projectiles.*/
	UFUNCTION(Server, Reliable)
	void ServerHandleFire(bool bShouldFire);

	void FireGun();

	bool bCanFire = true;

	float TimeBetweenProjectiles = 0.1f;
	FTimerHandle TimerHandle_TimeBetweenProjectiles;

	float FireCooldownPeriod = 0.25f;
	FTimerHandle TimerHandle_FireCooldownPeriod;

};
