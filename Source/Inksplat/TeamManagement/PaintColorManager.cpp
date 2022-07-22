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

	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*> Players;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), Players);
		UE_LOG(LogTemp, Warning, TEXT("Num of players found in manager is %i"), Players.Num());
		//AssignTeamColors(Players);
	}
}

void APaintColorManager::AssignTeamColors(const TArray<AActor*>& Players)
{
	////UGameplayStatics::GetActorsOfClass(GetWorld(), APlayerCharacter::StaticClass());
	//TArray<AActor*> Players;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), Players);
	//
	//for (AActor* Player : Players)
	//{
	//	if (Player->GetLocalRole() == ROLE_Authority)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Color Manager found %s in server"), *Player->GetName());
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Color Manager found %s in client"), *Player->GetName());
	//	}
	//}
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
	//if (GetLocalRole() == ROLE_Authority)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Called in server"));
	//}
	//else
	//{
	//	//If called in client, assign a color and temp color according to team name
	//}
}

