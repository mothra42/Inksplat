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

	SetIsReplicated(true);
}

void UPaintableStaticMeshComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPaintableStaticMeshComponent, ScanSpeed);
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


bool UPaintableStaticMeshComponent::PaintMesh(const FHitResult& Hit, const FLinearColor& Color)
{
	FVector2D UVPosition;
	UGameplayStatics::FindCollisionUV(Hit, UVChannelToPaint, UVPosition);
	FVector MaterialStretch;
	float MaterialScale;
	
	CalculateUVStretchAndScale(Hit, UVPosition, MaterialScale, MaterialStretch);
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

void UPaintableStaticMeshComponent::ScanMesh(float ScanSpeedToSet)
{
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		ScanSpeed = ScanSpeedToSet;
	}
}

void UPaintableStaticMeshComponent::OnRep_UpdateSpeedParam()
{
	if (GetOwner()->GetLocalRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Replicating ScanSpeed in On Rep"));
		UE_LOG(LogTemp, Warning, TEXT("ScanSpeed is %f"), ScanSpeed);
		MeshMaterialInstance->SetScalarParameterValue(FName("ScanSpeed"), ScanSpeed);
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_ScanTime,
			this,
			&UPaintableStaticMeshComponent::ProgressScan,
			ScanTime,
			true
		);
	}
}

void UPaintableStaticMeshComponent::StopScan()
{
	ScanSpeed = 0.0f;
}

void UPaintableStaticMeshComponent::ProgressScan()
{
	CurrentRange = FMath::Clamp(CurrentRange + 0.01f, 0.f, MaxRange);
	UE_LOG(LogTemp, Warning, TEXT("Progressing Scan and range is %f"), CurrentRange);
	if (CurrentRange >= MaxRange)
	{
		//reset range
		CurrentRange = 0.f;
		//clear timer
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ScanTime);
	}
	MeshMaterialInstance->SetScalarParameterValue(FName("ScanProgression"), CurrentRange);
}