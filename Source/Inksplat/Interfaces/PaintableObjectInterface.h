// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PaintableObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPaintableObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INKSPLAT_API IPaintableObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void PaintActor(const FHitResult& Hit, const FLinearColor& Color, bool bIsTemporary = false, float TempPaintLifetime = 0.f) { return; }
};
