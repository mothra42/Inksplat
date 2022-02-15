// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkeletalMeshPaintingLibrary.generated.h"


struct FVertexUVPair
{
    FVector2D UVs;
    FVector Position;
};

struct FSortVertexByDistance
{
    FSortVertexByDistance(const FVector& InSourceLocation)
        : SourceLocation(InSourceLocation)
    {

    }

    FVector SourceLocation;

    bool operator()(const FVertexUVPair A, const FVertexUVPair B) const
    {
        float DistanceA = FVector::DistSquared(SourceLocation, A.Position);
        float DistanceB = FVector::DistSquared(SourceLocation, B.Position);

        return DistanceA < DistanceB;
    }
};

/**
 * 
 */
UCLASS()
class INKSPLAT_API USkeletalMeshPaintingLibrary : public UObject
{
	GENERATED_BODY()
	
public:
    static bool FindCollisionUVFromHit(const struct FHitResult& Hit, FVector2D& UV);
};
