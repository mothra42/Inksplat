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
	FLinearColor ClearColor = FLinearColor(0.0, 0.0, 0.0, 1.0);
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

	//BrushMaterialInstance->SetScalarParameterValue(FName("Scale"), 1.0f);
	//BrushMaterialInstance->SetVectorParameterValue(FName("UVPosition"), FLinearColor(0.0, 0.0, 0.0));
	//BrushMaterialInstance->SetVectorParameterValue(FName("Stretch"), FLinearColor(0.1f, 0.1f, 1.0f));
	MeshMaterialInstance->SetTextureParameterValue(FName("ColorMap"), PaintTexture);
	SetMaterial(0, MeshMaterialInstance);
}


bool UPaintableStaticMeshComponent::PaintMesh(const FHitResult& Hit, const FLinearColor& Color)
{
	//FVector Scale = GetOwner()->GetActorScale3D();
	//UE_LOG(LogTemp, Warning, TEXT("Scale for object is %s"), *Scale.ToString());
	//FVector2D BaseSizePaintSquare(25.f, 25.f);
	//FVector2D CorrectPaintSquare = BaseSizePaintSquare * FVector2D(Scale.X, Scale.Y);
	//UCanvas* Canvas;
	FVector2D CanvasSize;
	FDrawToRenderTargetContext Context;
	FVector2D UVPosition;
	UGameplayStatics::FindCollisionUV(Hit, 0, UVPosition);

	FVector MaterialStretch;
	float MaterialScale;
	
	CalculateUVStretchAndScale(Hit, UVPosition, MaterialScale, MaterialStretch);
	//UE_LOG(LogTemp, Warning, TEXT("Stretch is %s"), *MaterialStretch.ToString());
	BrushMaterialInstance->SetVectorParameterValue(FName("UVPosition"), FLinearColor(UVPosition.X, UVPosition.Y, 0));
	BrushMaterialInstance->SetVectorParameterValue(FName("Stretch"), FLinearColor(MaterialStretch));
	BrushMaterialInstance->SetScalarParameterValue(FName("Scale"), MaterialScale);
	//if (BrushMaterialInstance != nullptr)
	//{
	//	BrushMaterialInstance->SetVectorParameterValue(FName("UVPosition"), FLinearColor(UVPosition.X, UVPosition.Y, 0.f));
	//	UE_LOG(LogTemp, Warning, TEXT("Is Valid"));
	//}
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), PaintTexture, BrushMaterialInstance);

	// OLD METHOD START
	//FVector2D BaseSplatSize(25.f, 25.f);
	//UE_LOG(LogTemp, Warning, TEXT("BaseSplatSize is %s"), *BaseSplatSize.ToString());
	//FVector2D CorrectedSplatSize = (BaseSplatSize / CalculatePaintScale(Hit.ImpactNormal));
	//UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, PaintTexture, Canvas, CanvasSize, Context);
	////TODO make this work for textures as well. Will probably need to include one in the constructor.
	////probably refactor this to exist in its own function
	//FVector2D TileItemPosition = CanvasSize * UVPosition;
	//FCanvasTileItem RectItem(
	//	TileItemPosition,
	//	CorrectedSplatSize, //size
	//	Color
	//);
	//Canvas->DrawItem(RectItem);
	////Canvas->DrawItem()
	//if (!this)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("this object is null"));
	//}
	//UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
	// OLD METHOD END
	



	return true;
}

void UPaintableStaticMeshComponent::CalculateUVStretchAndScale(const FHitResult& Hit, const FVector2D& UVPosition, float& OutScale, FVector& OutStretch)
{
	//TODO give variables better names and refactor method into more methods.
	FVector RandomVector = UKismetMathLibrary::RandomUnitVectorFromStream(FRandomStream(0));
	FVector OrthogonalVectorOne = FVector::CrossProduct(RandomVector, Hit.ImpactNormal).GetSafeNormal();
	FVector OrthogonalVectorTwo = FVector::CrossProduct(OrthogonalVectorOne, Hit.ImpactNormal).GetSafeNormal();

	FVector2D UVOffsetOne;
	FVector2D UVOffsetTwo;
	UGameplayStatics::FindCollisionUV(ConstructOffsetHitResult(OrthogonalVectorOne * 25.f, Hit.FaceIndex), 0, UVOffsetOne);
	UGameplayStatics::FindCollisionUV(ConstructOffsetHitResult(OrthogonalVectorTwo * 25.f, Hit.FaceIndex), 0, UVOffsetTwo);

	float UOffset = FVector2D::DotProduct(UVOffsetOne - UVPosition, FVector2D(1, 0));
	float VOffset = FVector2D::DotProduct(UVOffsetTwo - UVPosition, FVector2D(0, 1));
	OutStretch.X = UOffset;
	OutStretch.Y = VOffset;

	OutScale = (UVOffsetOne + UVOffsetOne).SizeSquared();
}

FHitResult UPaintableStaticMeshComponent::ConstructOffsetHitResult(FVector Location, int32 FaceIndex)
{
	FHitResult HitResult;
	HitResult.Component = this;
	HitResult.Location = Location;
	HitResult.FaceIndex = FaceIndex;

	return HitResult;
}

FVector2D UPaintableStaticMeshComponent::CalculatePaintScale(FVector Normal)
{
	float XAlignment = FMath::Abs(FVector::DotProduct(Normal, FVector::ForwardVector));
	float YAlignment = FMath::Abs(FVector::DotProduct(Normal, FVector::RightVector));
	float ZAlignment = FMath::Abs(FVector::DotProduct(Normal, FVector::UpVector));
	FVector MeshScale = GetComponentScale();
	if (XAlignment > YAlignment && XAlignment > ZAlignment)
	{
		//yz plane
		return FVector2D(MeshScale.Z, MeshScale.Y);
	}
	else if (YAlignment > XAlignment && YAlignment > ZAlignment)
	{
		//xz plane
		return FVector2D(MeshScale.Z, MeshScale.X);
	}
	else
	{
		//xy plane
		return FVector2D(MeshScale.X, MeshScale.Y);
	}
}
