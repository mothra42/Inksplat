// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableStaticMeshComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Canvas.h"
#include "Containers/Queue.h"

UPaintableStaticMeshComponent::UPaintableStaticMeshComponent()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> PaintableMaterialFinder(
		TEXT("/Game/PaintableMaterials/M_PaintableMaterial")
	);

	static ConstructorHelpers::FObjectFinder<UMaterial> BrushMaterialFinder(
		TEXT("/Game/PaintableMaterials/M_Brush")
	);

	if (PaintableMaterialFinder.Succeeded())
	{
		ParentMaterial = PaintableMaterialFinder.Object;
	}
	if (BrushMaterialFinder.Succeeded())
	{
		BrushMaterial = BrushMaterialFinder.Object;
	}
}

void UPaintableStaticMeshComponent::BeginPlay()
{
	Super::BeginPlay();
	FLinearColor ClearColor = FLinearColor(0.0, 0.0, 0.0, 0.0);
	PaintTexture = UKismetRenderingLibrary::CreateRenderTarget2D(
		GetWorld(),
		2048,
		2048,
		RTF_RGBA16f,
		ClearColor
	);

	MeshMaterialInstance = UMaterialInstanceDynamic::Create(
		ParentMaterial,
		this
	);

	BrushMaterialInstance = UMaterialInstanceDynamic::Create(
		BrushMaterial,
		this
	);

	MeshMaterialInstance->SetTextureParameterValue(FName("ColorMap"), PaintTexture);
	SetMaterial(0, MeshMaterialInstance);
}


bool UPaintableStaticMeshComponent::PaintMesh(const FHitResult& Hit, const FLinearColor& Color)
{
	FVector2D UVPosition;
	UGameplayStatics::FindCollisionUV(Hit, 0, UVPosition);

	FVector MaterialStretch;
	float MaterialScale;
	
	CalculateUVStretchAndScale(Hit, UVPosition, MaterialScale, MaterialStretch);
	BrushMaterialInstance->SetVectorParameterValue(FName("UVTransform"), FLinearColor(UVPosition.X, UVPosition.Y, 0));
	BrushMaterialInstance->SetVectorParameterValue(FName("Stretch"), FLinearColor(MaterialStretch));
	BrushMaterialInstance->SetScalarParameterValue(FName("Scale"), MaterialScale);
	UE_LOG(LogTemp, Warning, TEXT("Material Scale is %f"), MaterialScale);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), PaintTexture, BrushMaterialInstance);

	return true;
}

void UPaintableStaticMeshComponent::CalculateUVStretchAndScale(const FHitResult& Hit, const FVector2D& UVPosition, float& OutScale, FVector& OutStretch)
{
	//TODO give variables better names and refactor method into more methods.
	FVector RandomVector = UKismetMathLibrary::RandomUnitVectorFromStream(FRandomStream(0)).GetSafeNormal();
	FVector OrthogonalVectorOne = FVector::CrossProduct(RandomVector, Hit.Normal).GetSafeNormal();
	FVector OrthogonalVectorTwo = FVector::CrossProduct(OrthogonalVectorOne, Hit.Normal).GetSafeNormal();

	FVector2D UVOffsetOne;
	FVector2D UVOffsetTwo;
	UGameplayStatics::FindCollisionUV(ConstructOffsetHitResult(((OrthogonalVectorOne * 25.f) + Hit.Location), Hit.FaceIndex), 0, UVOffsetOne);
	UGameplayStatics::FindCollisionUV(ConstructOffsetHitResult(((OrthogonalVectorTwo * 25.f) + Hit.Location), Hit.FaceIndex), 0, UVOffsetTwo);

	OutScale = ((UVOffsetOne - UVPosition) + (UVOffsetTwo - UVPosition)).Size();

	float UOffset = FVector2D::DotProduct(UVOffsetOne - UVPosition, FVector2D(1, 0));
	float VOffset = FVector2D::DotProduct(UVOffsetTwo - UVPosition, FVector2D(0, 1));
	OutStretch.X = UOffset;
	OutStretch.Y = VOffset;
}

FHitResult UPaintableStaticMeshComponent::ConstructOffsetHitResult(FVector Location, int32 FaceIndex)
{
	FHitResult HitResult;
	HitResult.Component = this;
	HitResult.Location = Location;
	HitResult.FaceIndex = FaceIndex;

	return HitResult;
}

FVector UPaintableStaticMeshComponent::CalculatePaintScale(FVector Normal)
{
	float XAlignment = FMath::Abs(FVector::DotProduct(Normal, FVector::ForwardVector));
	float YAlignment = FMath::Abs(FVector::DotProduct(Normal, FVector::RightVector));
	float ZAlignment = FMath::Abs(FVector::DotProduct(Normal, FVector::UpVector));
	FVector MeshScale = GetComponentScale();
	if (XAlignment > YAlignment && XAlignment > ZAlignment)
	{
		//yz plane
		return FVector(MeshScale.Z, MeshScale.Y, 0);
	}
	else if (YAlignment > XAlignment && YAlignment > ZAlignment)
	{
		//xz plane
		return FVector(MeshScale.Z, MeshScale.X, 0);
	}
	else
	{
		//xy plane
		return FVector(MeshScale.X, MeshScale.Y, 0);
	}
}