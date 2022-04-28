// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletalMeshPaintingLibrary.h"
#include "Rendering/SkeletalMeshRenderData.h"


bool USkeletalMeshPaintingLibrary::FindCollisionUVFromHit(const struct FHitResult& Hit, FVector2D& UV)
{
    FVector PointInReferencePose;
    if (UPrimitiveComponent* HitPrimComp = Hit.Component.Get())
    {
        TArray<FVertexUVPair> AllVertices;
        const FVector LocalHitPos = HitPrimComp->GetComponentTransform().InverseTransformPosition(Hit.Location);
            //HitPrimComp->GetComponentToWorld().InverseTransformPosition(Hit.Location);

        if (USkeletalMeshComponent* SkelComp = Cast<USkeletalMeshComponent>(HitPrimComp))
        {
            if (FSkeletalMeshRenderData* RenderData = SkelComp->GetSkeletalMeshRenderData())
            {
                if (RenderData->LODRenderData.Num() > 0)
                {
                    for (uint32 i = 0; i < RenderData->LODRenderData[0].StaticVertexBuffers.StaticMeshVertexBuffer.GetNumVertices(); ++i)
                    {
                        FVertexUVPair NewPair;
                        NewPair.Position = RenderData->LODRenderData[0].StaticVertexBuffers.PositionVertexBuffer.VertexPosition(i);
                        NewPair.UVs = RenderData->LODRenderData[0].StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(i, 0);
                        AllVertices.Emplace(NewPair);
                    }
                }

                GetHitResultPointInRefPose(Hit, PointInReferencePose);
            }
        }
        else if (UBodySetup* BodySetup = HitPrimComp->GetBodySetup())
        {
            const int32 VertexCount = BodySetup->UVInfo.VertPositions.Num();
            for (int32 Index = 0; Index < VertexCount; Index++)
            {
                FVertexUVPair NewPair;
                NewPair.Position = BodySetup->UVInfo.VertPositions[Index];
                NewPair.UVs = BodySetup->UVInfo.VertUVs[0][Index];
                AllVertices.Emplace(NewPair);
            }
        }

        // Sort Shortest Vertex from LocalHitPos
        AllVertices.Sort(FSortVertexByDistance(PointInReferencePose));

        if (AllVertices.Num() < 7)
        {
            return false;
        }

        // Get three vertex for computing barycentric coords
        FVector Pos0 = AllVertices[0].Position;
        FVector Pos1 = AllVertices[3].Position;
        FVector Pos2 = AllVertices[6].Position;

        FVector2D UV0 = AllVertices[0].UVs;
        FVector2D UV1 = AllVertices[3].UVs;
        FVector2D UV2 = AllVertices[6].UVs;

        // Transform hit location from world to local space.
        // Find barycentric coords
        FVector BaryCoords = FMath::ComputeBaryCentric2D(PointInReferencePose, Pos0, Pos1, Pos2);
        // Use to blend UVs
        UV = (BaryCoords.X * UV0) + (BaryCoords.Y * UV1) + (BaryCoords.Z * UV2);

        return true;
    }

    return false;
}

FTransform USkeletalMeshPaintingLibrary::GetSkeletalMeshRefPose(USkeletalMesh* SkeletalMesh,int BoneIndex)
{
    FTransform MeshBonePoseTransform;
    MeshBonePoseTransform.SetIdentity();
    MeshBonePoseTransform = SkeletalMesh->RefSkeleton.GetRefBonePose()[BoneIndex];
    int ParentBoneIndex = SkeletalMesh->RefSkeleton.GetParentIndex(BoneIndex);
    if (ParentBoneIndex == INDEX_NONE)
    {
        return MeshBonePoseTransform;
    }
    else
    {
        return MeshBonePoseTransform * GetSkeletalMeshRefPose(SkeletalMesh, ParentBoneIndex);
    }
}

bool USkeletalMeshPaintingLibrary::GetHitResultPointInRefPose(const struct FHitResult& HitResult, FVector& Point)
{
    if (UPrimitiveComponent* HitPrimComp = HitResult.Component.Get())
    {
        FVector HitPoint = HitResult.ImpactPoint;
        if (USkeletalMeshComponent* SkelComp = Cast<USkeletalMeshComponent>(HitPrimComp))
        {
            int BoneIndex = SkelComp->SkeletalMesh->RefSkeleton.FindBoneIndex(HitResult.BoneName);
            FTransform PoseTransform = SkelComp->SkeletalMesh->RefSkeleton.GetRefBonePose()[BoneIndex];
            HitPoint = SkelComp->GetSocketTransform(HitResult.BoneName).InverseTransformPosition(HitPoint);
            Point = GetSkeletalMeshRefPose(SkelComp->SkeletalMesh, BoneIndex).TransformPosition(HitPoint);
            return true;
        }
    }
    return false;
}