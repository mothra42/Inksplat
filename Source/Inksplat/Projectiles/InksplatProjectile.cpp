// Copyright Epic Games, Inc. All Rights Reserved.

#include "InksplatProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../PaintableActorComponents/PaintableObjectComponent.h"
#include "Components/SphereComponent.h"

AInksplatProjectile::AInksplatProjectile() 
{
	//Make projectile a replicated actor
	bReplicates = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	if (GetLocalRole() == ROLE_Authority)
	{
		// set up a notification for when this component hits something blocking
		CollisionComp->OnComponentHit.AddDynamic(this, &AInksplatProjectile::OnProjectileImpact);
	}
			
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AInksplatProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	UPaintableObjectComponent* PaintableObjectComp = Cast<UPaintableObjectComponent>(OtherActor->FindComponentByClass(UPaintableObjectComponent::StaticClass()));
	if (PaintableObjectComp != nullptr)
	{
		PaintableObjectComp->FindAvailablePaintQueue(Hit, 1.0f);
		UE_LOG(LogTemp, Warning, TEXT("Actor hit is %s"), *OtherActor->GetName());
	}

	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}

void AInksplatProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	UPaintableObjectComponent* PaintableObjectComp = Cast<UPaintableObjectComponent>(OtherActor->FindComponentByClass(UPaintableObjectComponent::StaticClass()));
	if (PaintableObjectComp != nullptr)
	{	
		PaintableObjectComp->FindAvailablePaintQueue(Hit, 1.0f);
		UE_LOG(LogTemp, Warning, TEXT("Actor hit is %s"), *OtherActor->GetName());
	}

	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}