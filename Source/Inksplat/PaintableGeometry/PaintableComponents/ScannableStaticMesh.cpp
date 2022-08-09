// Fill out your copyright notice in the Description page of Project Settings.


#include "ScannableStaticMesh.h"

UScannableStaticMesh::UScannableStaticMesh()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> PaintableMaterialFinder(
		TEXT("/Game/PaintableMaterials/M_ScanMaterial")
	);

	if (PaintableMaterialFinder.Succeeded())
	{
		ParentMaterial = PaintableMaterialFinder.Object;
	}

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UScannableStaticMesh::BeginPlay()
{
	Super::BeginPlay();

	MeshMaterialInstance = UMaterialInstanceDynamic::Create(
		ParentMaterial,
		this
	);

	SetMaterial(0, MeshMaterialInstance);
}

void UScannableStaticMesh::ScanMesh(const float ScanSpeed, const float RangeToSet)
{
	MaxRange = RangeToSet;

	MeshMaterialInstance->SetScalarParameterValue(FName("ScanSpeed"), ScanSpeed);
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_ScanTime,
		this,
		&UScannableStaticMesh::ProgressScan,
		ScanTime,
		true
	);
}

void UScannableStaticMesh::ProgressScan()
{
	CurrentRange = FMath::Clamp(CurrentRange + 0.01f, 0.f, MaxRange);
	if (CurrentRange >= MaxRange)
	{
		//reset range
		CurrentRange = 0.f;
		//clear timer
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ScanTime);
	}
	MeshMaterialInstance->SetScalarParameterValue(FName("ScanProgression"), CurrentRange);
}

