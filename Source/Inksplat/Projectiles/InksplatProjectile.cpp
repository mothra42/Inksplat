// Copyright Epic Games, Inc. All Rights Reserved.

#include "InksplatProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../PaintableActorComponents/PaintableObjectComponent.h"
#include "../PaintableGeometry/PaintableActorBase.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

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

	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		PaintActor(Hit);
		Destroy();
	}
}

void AInksplatProjectile::Destroyed()
{
	
	//need to think about where this function runs and its role
	//PaintActor();
	//TODO make paint particle effect spawn here.
}

void AInksplatProjectile::PaintActor(const FHitResult& Hit)
{
	FVector TraceBeginLocation = GetActorLocation();
	FVector TraceDirection = UKismetMathLibrary::GetDirectionUnitVector(TraceBeginLocation, Hit.Location);
	FVector TraceEndLocation = TraceBeginLocation + 100 * TraceDirection;

	FHitResult LineTraceHit;
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(this);
	TraceParams.bReturnFaceIndex = true;
	GetWorld()->LineTraceSingleByChannel(
		LineTraceHit,
		TraceBeginLocation,
		TraceEndLocation,
		ECC_Visibility,
		TraceParams
	);

	APaintableActorBase* PaintableActor = Cast<APaintableActorBase>(LineTraceHit.Actor);
	if (PaintableActor)
	{
		PaintableActor->PaintActor(LineTraceHit, FLinearColor::Blue);
	}
}