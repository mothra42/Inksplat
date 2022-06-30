// Fill out your copyright notice in the Description page of Project Settings.


#include "ForcePushAbilityComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../PlayerActor/PlayerCharacter.h"
#include "../PaintableGeometry/PaintableComponents/PaintableSkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UForcePushAbilityComponent::UForcePushAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}


// Called when the game starts
void UForcePushAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UForcePushAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UForcePushAbilityComponent, bCanUseAbility);
}


// Called every frame
void UForcePushAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UForcePushAbilityComponent::UseAbility()
{
	//if the ability is useable call server execute ability
	UE_LOG(LogTemp, Warning, TEXT("Executing Ability"));
	UE_LOG(LogTemp, Warning, TEXT("Owner is %s"), *GetOwner()->GetName());
	if (bCanUseAbility)
	{
		ServerExecuteAbility();
	}
	return;
}

void UForcePushAbilityComponent::ServerExecuteAbility_Implementation()
{
	bCanUseAbility = false;
	UE_LOG(LogTemp, Warning, TEXT("Executing on server"));
	//TODO do physics things to objects on server here.
	TSet<APawn*> AffectedPawns = FindAffectedPawns();
	ApplyImpulseToPawns(AffectedPawns);
	////start cooldown

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_AbilityCooldown,
		this,
		&UForcePushAbilityComponent::ResetAfterCoolDown,
		CoolDownTime,
		false
	);

}

TSet<APawn*> UForcePushAbilityComponent::FindAffectedPawns()
{
	TSet<APawn*> AffectedPawns;
	TArray<FHitResult> OutHits;
	FVector StartPosition = GetOwner()->GetActorLocation() + 
		GetOwner()->GetActorForwardVector() * BoxTraceOffset;
	FVector EndPosition = StartPosition + FVector(0.1, 0.1, 0.1);
	FRotator OwnerRotation = GetOwner()->GetActorRotation();
	UKismetSystemLibrary::BoxTraceMultiByProfile(
		GetWorld(),
		StartPosition,
		EndPosition,
		FVector(BoxLength, BoxWidth, BoxHeight),
		FRotator(0, OwnerRotation.Yaw, 0),
		FName("Pawn"),
		false,
		TArray<AActor*>{GetOwner()},
		EDrawDebugTrace::None,
		OutHits,
		true
	);

	//TODO this could be further optimized, since each physics component counts as a hit.
	for (FHitResult HitResult : OutHits)
	{
		APawn* PawnToAdd = Cast<APawn>(HitResult.Actor);
		if (PawnToAdd != nullptr)
		{
			AffectedPawns.Add(PawnToAdd);
		}
	}
	return AffectedPawns;
}

//TODO give this method a better name like ApplyImpusleAndPaint
void UForcePushAbilityComponent::ApplyImpulseToPawns(const TSet<APawn*>& AffectedPawns)
{
	for (APawn* Pawn : AffectedPawns)
	{
		APlayerCharacter* OtherPlayer = Cast<APlayerCharacter>(Pawn);
		if (OtherPlayer != nullptr)
		{
			ApplyTempPaintToHitPlayer(OtherPlayer);

			FVector ForceToApply = CalculateForceToApply(Pawn->GetActorLocation());
			OtherPlayer->GetCharacterMovement()->AddImpulse(ForceToApply);
		}
	}
}

FVector UForcePushAbilityComponent::CalculateForceToApply(FVector PawnLocation)
{
	FVector ForceOrigin = GetOwner()->GetActorLocation();
	float Radius = FVector::Distance(ForceOrigin, PawnLocation);
	//TODO make clamp values customizeable in BP
	float CorrectedRadius = FMath::Clamp(Radius, 200.f, 240.f);
	FVector ForceDirection = ((PawnLocation - ForceOrigin) + FVector(0, 0, 100)).GetSafeNormal();
	DrawDebugLine(GetWorld(), ForceOrigin,
		ForceOrigin + (ForceDirection * 500), FColor::Green, false, 5.0f);
	return ForceDirection * (BaseForce / CorrectedRadius);
}

void UForcePushAbilityComponent::ApplyTempPaintToHitPlayer_Implementation(APlayerCharacter* OtherPlayer)
{
	FVector TraceBeginLocation = GetOwner()->GetActorLocation();
	FVector TraceEndLocation = OtherPlayer->GetActorLocation();

	FHitResult LineTraceHit;
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(GetOwner());
	TraceParams.bReturnFaceIndex = true;
	GetWorld()->LineTraceSingleByChannel(
		LineTraceHit,
		TraceBeginLocation,
		TraceEndLocation,
		ECC_Visibility,
		TraceParams
	);

	OtherPlayer->PaintActor(LineTraceHit, FColor::Red, false);
}

void UForcePushAbilityComponent::ResetAfterCoolDown()
{
	UE_LOG(LogTemp, Warning, TEXT("Cooldown is over"));
	bCanUseAbility = true;
}

void UForcePushAbilityComponent::OnRep_CanUseAbility()
{
	//TODO update UI from this method.
}

// Force push should work as a simple call to execute a method on the server, much like firing a gun.

//Force push therefore needs an execution method that makes force push happen on server
	//this will need to be a server RPC call

//Force push also needs a cooldown timer that can be adjusted from blueprint for balancing.
	//similar to weapon firing methods

//force push needs several variables to tweak for flexibility
	//The amount of force to apply
	//Valid objects that can be pushed
	//room for animation and VFX
	//A Mesh that looks for objects that need to have force applied to them.

//It would be best if all abilities inherited from a basic ability interface that mandates
//a cool down and an execution method. Thos are the two things every ability should have.