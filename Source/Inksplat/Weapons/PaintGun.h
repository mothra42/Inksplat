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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Server_FireProjectile();

	void SetOwningPlayer(APlayerCharacter* NewPlayerOwner);

};
