// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableSkeletalMeshComponent.h"
#include "../HelperActors/PaintHelper.h"
#include "../../CustomStaticMethods/SkeletalMeshPaintingLibrary.h"
#include "../../PlayerActor/PlayerCharacter.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Containers/Queue.h"
#include "DrawDebugHelpers.h"

UPaintableSkeletalMeshComponent::UPaintableSkeletalMeshComponent()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> PaintableMaterialFinder(
		TEXT("/Game/PaintableMaterials/M_PaintableMaterial_PlayerActor")
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

	PaintCoverageArray.Init(0, 2048);
}

void UPaintableSkeletalMeshComponent::BeginPlay()
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

	TempPaintTexture = UKismetRenderingLibrary::CreateRenderTarget2D(
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
	MeshMaterialInstance->SetTextureParameterValue(FName("TempColorMap"), TempPaintTexture);
	SetMaterial(0, MeshMaterialInstance);
}

void UPaintableSkeletalMeshComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPaintableSkeletalMeshComponent, NumPaintedTiles);

}

bool UPaintableSkeletalMeshComponent::PaintMesh(const FHitResult& Hit, const FLinearColor& Color, bool bIsTemporary, float TempPaintLifetime)
{
	FVector2D UVPosition;
	USkeletalMeshPaintingLibrary::FindCollisionUVFromHit(Hit, UVPosition);

	BrushMaterialInstance->SetVectorParameterValue(FName("UVTransform"), FLinearColor(UVPosition.X, UVPosition.Y, 0));
	BrushMaterialInstance->SetVectorParameterValue(FName("Stretch"), FLinearColor(MaterialStretch));
	BrushMaterialInstance->SetScalarParameterValue(FName("Scale"), MaterialScale);
	BrushMaterialInstance->SetVectorParameterValue(FName("TintColor"), Color);
	if (PaintHelper)
	{
		BrushMaterialInstance->SetTextureParameterValue(FName("PaintTexture"), PaintHelper->GetPaintSplatTexture());
	}

	if (!bIsTemporary)
	{
		UE_LOG(LogTemp, Warning, TEXT("Painting Perm"));
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), PaintTexture, BrushMaterialInstance);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Painting Temp"));
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), TempPaintTexture, BrushMaterialInstance);
		//TODO erase added textue after X time.
	}
	
	return true;
}

void UPaintableSkeletalMeshComponent::CalculateUVStretchAndScale(const FHitResult& Hit, const FVector2D& UVPosition, float& OutScale, FVector& OutStretch)
{
	//TODO give variables better names and refactor method into more methods.
	FVector RandomVector = UKismetMathLibrary::RandomUnitVectorFromStream(FRandomStream(0)).GetSafeNormal();
	FVector OrthogonalVectorOne = FVector::CrossProduct(RandomVector, Hit.Normal).GetSafeNormal();
	FVector OrthogonalVectorTwo = FVector::CrossProduct(OrthogonalVectorOne, Hit.Normal).GetSafeNormal();

	FVector2D UVOffsetOne;
	FVector2D UVOffsetTwo;
	DrawDebugSphere(GetWorld(), Hit.Location, 3, 8, FColor::Red, false, 3.0f);
	DrawDebugSphere(GetWorld(), (OrthogonalVectorOne * 10.f) + Hit.Location, 3, 8, FColor::Blue, false, 3.0f);
	DrawDebugSphere(GetWorld(), (OrthogonalVectorTwo * 10.f) + Hit.Location, 3, 8, FColor::Green, false, 3.0f);
	USkeletalMeshPaintingLibrary::FindCollisionUVFromHit(ConstructOffsetHitResult(((OrthogonalVectorOne * 5.f) + Hit.Location), Hit.FaceIndex, Hit.BoneName), UVOffsetOne);
	USkeletalMeshPaintingLibrary::FindCollisionUVFromHit(ConstructOffsetHitResult(((OrthogonalVectorTwo * 5.f) + Hit.Location), Hit.FaceIndex, Hit.BoneName), UVOffsetTwo);
	//UGameplayStatics::FindCollisionUV(ConstructOffsetHitResult(((OrthogonalVectorOne * 3.f) + Hit.Location), Hit.FaceIndex), 0, UVOffsetOne);
	//UGameplayStatics::FindCollisionUV(ConstructOffsetHitResult(((OrthogonalVectorTwo * 3.f) + Hit.Location), Hit.FaceIndex), 0, UVOffsetTwo);

	OutScale = ((UVOffsetOne - UVPosition) + (UVOffsetTwo - UVPosition)).Size();

	float UOffset = FVector2D::DotProduct(UVOffsetOne - UVPosition, FVector2D(1, 0));
	float VOffset = FVector2D::DotProduct(UVOffsetTwo - UVPosition, FVector2D(0, 1));
	OutStretch.X = UOffset;
	OutStretch.Y = VOffset;
	OutStretch.Z = 0;
}

FHitResult UPaintableSkeletalMeshComponent::ConstructOffsetHitResult(FVector Location, int32 FaceIndex, FName BoneName)
{
	FHitResult HitResult;
	HitResult.Component = this;
	HitResult.Location = Location;
	HitResult.FaceIndex = FaceIndex;
	HitResult.BoneName = BoneName;

	return HitResult;
}

//TODO Consider adding a fourth argument that is a reference to a reference texture to check for the bounds of the UV map
float UPaintableSkeletalMeshComponent::CalculatePaintCoverage(FVector2D Origin)
{
	//TODO revisit this calculation to work in line with new painting method
	/*A Note on some magic numbers being used. 1024 is the size of the texture both U and V.
	* 1024 * 1024 is 1048576. If I paint squares that are 22.62 X 22.62 then the total pixel count is 512
	* 1048567 / 512 is 2048. So there are 2048 distinct tiles of 512 pixels. Which is where these numbers come from.
	* TODO codify these magic numbers into actual expressions.
	*/
	int32 PaintedTile = (Origin.X + Origin.Y * 1024) / 512;
	if (PaintedTile < 0 || PaintedTile >= 2048)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Invalid Tile"));
		return NumPaintedTiles / MaxPaintedTiles;
	}
	else if(!PaintCoverageArray[PaintedTile])
	{
		NumPaintedTiles++;
		PaintCoverageArray[PaintedTile] = 1;
	}

	UpdateParentHealth();
	return NumPaintedTiles / MaxPaintedTiles;
}

void UPaintableSkeletalMeshComponent::UpdateParentHealth()
{
	APlayerCharacter* PlayerOwner = Cast<APlayerCharacter>(GetOwner());
	if (PlayerOwner)
	{
		float test = float(NumPaintedTiles) / float(MaxPaintedTiles);
		UE_LOG(LogTemp, Warning, TEXT("Percentage from mesh is %f"), test);
		PlayerOwner->OnHealthUpdate(float(NumPaintedTiles) / MaxPaintedTiles);
	}
}

void UPaintableSkeletalMeshComponent::OnRep_NumPaintedTiles()
{
	UE_LOG(LogTemp, Warning, TEXT("NumPaintedTiles is %i"), NumPaintedTiles);
	UE_LOG(LogTemp, Warning, TEXT("MaxPaintedTiles is %i"), MaxPaintedTiles);
	UpdateParentHealth();
}
