// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintColorManager.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "../PlayerActor/PlayerCharacter.h"

// Sets default values
APaintColorManager::APaintColorManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;
}

// Called when the game starts or when spawned
void APaintColorManager::BeginPlay()
{
	Super::BeginPlay();
}


//Currently called from the BP version of this class on a delay.
//Ideally this method is called after all players are loaded in and before an actual match begins.
//TODO add a hook for this method as a sort of BeginMatch()
void APaintColorManager::AssignTeamColors(const TArray<AActor*>& Players)
{
	for (AActor* Player : Players)
	{
		APlayerCharacter* CurrentPlayer = Cast<APlayerCharacter>(Player);
		if (CurrentPlayer != nullptr)
		{
			CurrentPlayer->SetTeamPaintColor(TeamOneColor);
			CurrentPlayer->SetTeamTempPaintColor(TeamOneTempColor);
		}
		if (Player->GetLocalRole() == ROLE_Authority)
		{
			UE_LOG(LogTemp, Warning, TEXT("Color is %s in server"), *TeamOneColor.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Color is %s in client"), *TeamOneColor.ToString());
		}
	}
}

