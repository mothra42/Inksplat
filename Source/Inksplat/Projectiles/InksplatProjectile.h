// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InksplatProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class AInksplatProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;


protected:
	UPROPERTY(ReplicatedUsing=OnRep_MeshHit)
	FVector HitLocation;

	UPROPERTY(ReplicatedUsing = OnRep_PaintColor)
	FColor PaintColor;

protected:
	virtual void BeginPlay() override;

	//called when projectile impacts actor
	UFUNCTION(Category = "Projectile")
	void OnProjectileImpact(
		UPrimitiveComponent* HitComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		FVector NormalImpulse, 
		const FHitResult& Hit
	);

	UFUNCTION()
	void OnRep_MeshHit();

	UFUNCTION()
	void OnRep_PaintColor();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Destroyed() override;

	//Paint the actor should only occur locally and not in server
	void PaintActor();
public:
	AInksplatProjectile();

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	void SetProjectilePaintColor(FColor ColorToSet) { PaintColor = ColorToSet; }
};

