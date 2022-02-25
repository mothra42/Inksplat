// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/PaintableObjectInterface.h"
#include "PaintableActorBase.generated.h"

UCLASS()
class INKSPLAT_API APaintableActorBase : public AActor, public IPaintableObjectInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APaintableActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class USceneComponent* Root;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PaintActor(const FHitResult& Hit, const FLinearColor& Color) override;

};
