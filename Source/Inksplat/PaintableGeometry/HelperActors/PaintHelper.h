// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaintHelper.generated.h"

UCLASS()
class INKSPLAT_API APaintHelper : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APaintHelper();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Category="Paint Textures", EditDefaultsOnly)
	TArray<class UTexture*> PaintSplatTextures;

public:	
	UTexture* GetPaintSplatTexture();

};
