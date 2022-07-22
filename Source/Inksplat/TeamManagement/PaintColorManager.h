// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaintColorManager.generated.h"

USTRUCT()
struct FTeamColors
{
	GENERATED_BODY()

	UPROPERTY()
	FColor TeamColor;

	UPROPERTY()
	FColor TeamTempColor;

	FTeamColors()
	{
		TeamColor = FColor(1, 0, 0, 1);
		TeamTempColor = FColor(0, 1, 0);
	}

	FTeamColors(FColor Color, FColor TempColor)
	{
		TeamColor = Color;
		TeamTempColor = TempColor;
	}
};

UCLASS()
class INKSPLAT_API APaintColorManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APaintColorManager();
protected:
	class USceneComponent* SceneComponent;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Category = "TeamColor", EditDefaultsOnly)
	FColor TeamOneColor = FColor(1, 0, 0);

	UPROPERTY(Category = "TeamColor", EditDefaultsOnly)
	FColor TeamOneTempColor = FColor(1, 0, 0);

	UPROPERTY(Category = "TeamColor", EditDefaultsOnly)
	FColor TeamTwoColor = FColor(0, 1, 0);

	UPROPERTY(Category = "TeamColor", EditDefaultsOnly)
	FColor TeamTwoTempColor = FColor(0, 1, 0);

public:	
	UFUNCTION(BlueprintCallable)
	void AssignTeamColors(const TArray<AActor*>& Players);
};
