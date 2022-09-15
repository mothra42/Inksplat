// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableStaticMeshComponent.h"
#include "../HelperActors/PaintHelper.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
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

	bPaintHitLocationOnly = true;

	PaintCoverageArray.Init(0, 2048);

	SetIsReplicated(true);
}

void UPaintableStaticMeshComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPaintableStaticMeshComponent, NumPaintedTiles);
	DOREPLIFETIME(UPaintableStaticMeshComponent, LastHitLocation);
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
	
	PaintHelper = Cast<APaintHelper>(UGameplayStatics::GetActorOfClass(GetWorld(), APaintHelper::StaticClass()));

	MeshMaterialInstance->SetTextureParameterValue(FName("ColorMap"), PaintTexture);
	SetMaterial(0, MeshMaterialInstance);
}


bool UPaintableStaticMeshComponent::PaintMesh(const FHitResult& Hit, const FLinearColor& Color, const float ScaleModifier)
{
	FVector2D UVPosition;
	UGameplayStatics::FindCollisionUV(Hit, UVChannelToPaint, UVPosition);
	//LastHitLocation = UVPosition;
	FVector MaterialStretch;
	float MaterialScale;

	CalculateUVStretchAndScale(Hit, UVPosition, MaterialScale, MaterialStretch);
	MaterialScale *= ScaleModifier;
	
	//If the material should be painted fully even if only one spot is hit.
	if (!bPaintHitLocationOnly)
	{
		//TODO work on this scale modifier math.
		MaterialScaleModifier += (0.5 * MaterialScaleModifier);
		MaterialScale *= MaterialScaleModifier;
	}

	BrushMaterialInstance->SetVectorParameterValue(FName("UVTransform"), FLinearColor(UVPosition.X, UVPosition.Y, 0));
	BrushMaterialInstance->SetVectorParameterValue(FName("Stretch"), FLinearColor(MaterialStretch));
	BrushMaterialInstance->SetScalarParameterValue(FName("Scale"), MaterialScale);
	BrushMaterialInstance->SetVectorParameterValue(FName("TintColor"), Color);
	if (PaintHelper)
	{
		BrushMaterialInstance->SetTextureParameterValue(FName("PaintTexture"), PaintHelper->GetPaintSplatTexture());
	}

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
	UGameplayStatics::FindCollisionUV(ConstructOffsetHitResult(((OrthogonalVectorOne * 25.f) + Hit.Location), Hit.FaceIndex), UVChannelToPaint, UVOffsetOne);
	UGameplayStatics::FindCollisionUV(ConstructOffsetHitResult(((OrthogonalVectorTwo * 25.f) + Hit.Location), Hit.FaceIndex), UVChannelToPaint, UVOffsetTwo);

	OutScale = ((UVOffsetOne - UVPosition) + (UVOffsetTwo - UVPosition)).Size();

	float UOffset = FVector2D::DotProduct(UVOffsetOne - UVPosition, FVector2D(1, 0));
	float VOffset = FVector2D::DotProduct(UVOffsetTwo - UVPosition, FVector2D(0, 1));
	OutStretch.X = UOffset;
	OutStretch.Y = VOffset;
	OutStretch.Z = 0;
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

void UPaintableStaticMeshComponent::OnRep_LastHitLocation()
{
	//if (GetOwner()->GetLocalRole() == ROLE_Authority)
	//{
		//UE_LOG(LogTemp, Warning, TEXT("Calculating in the server"));
		//CalculatePaintCoverage();
	//}
}

float UPaintableStaticMeshComponent::CalculatePaintCoverage(const FHitResult& Hit)
{
	//TODO revisit this calculation to work in line with new painting method
	/*A Note on some magic numbers being used. 1024 is the size of the texture both U and V.
	* 1024 * 1024 is 1048576. If I paint squares that are 22.62 X 22.62 then the total pixel count is 512
	* 1048567 / 512 is 2048. So there are 2048 distinct tiles of 512 pixels. Which is where these numbers come from.
	* TODO codify these magic numbers into actual expressions.
	*/
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		FVector2D UVPosition;
		UGameplayStatics::FindCollisionUV(Hit, UVChannelToPaint, UVPosition);

		int32 PaintedTile = (UVPosition.X + UVPosition.Y * 1024) / 512;
		if (PaintedTile < 0 || PaintedTile >= 2048)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Invalid Tile"));
			return NumPaintedTiles / MaxPaintedTiles;
		}
		else if (!PaintCoverageArray[PaintedTile])
		{
			NumPaintedTiles++;
			PaintCoverageArray[PaintedTile] = 1;
		}

		//UpdateParentHealth();
		UE_LOG(LogTemp, Warning, TEXT("NumPaintedTiles is %i"), NumPaintedTiles);
		UE_LOG(LogTemp, Warning, TEXT("Ratio is %f"), NumPaintedTiles / MaxPaintedTiles);
		return NumPaintedTiles / MaxPaintedTiles;
	}

	return 0.f;
}

void UPaintableStaticMeshComponent::OnRep_NumPaintedTiles()
{
	//UpdateParentHealth();
}
