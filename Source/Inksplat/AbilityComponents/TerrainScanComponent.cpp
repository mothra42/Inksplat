// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainScanComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../PlayerActor/PlayerCharacter.h"
#include "../PaintableGeometry/PaintableComponents/PaintableStaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

UTerrainScanComponent::UTerrainScanComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

void UTerrainScanComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTerrainScanComponent::UseAbility()
{
	UE_LOG(LogTemp, Warning, TEXT("Using Terrain Scan"));
	ExecuteAbility_BPTest();
}

void UTerrainScanComponent::ResetAfterCoolDown()
{

}