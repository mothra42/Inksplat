// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableActorBase.h"
#include "PaintableComponents/PaintableStaticMeshComponent.h"
#include "PaintableComponents/PaintableSkeletalMeshComponent.h"
#include "PaintableComponents/ScannableStaticMesh.h"

// Sets default values
APaintableActorBase::APaintableActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	bReplicates = true;
	RootComponent = Root;
}

// Called when the game starts or when spawned
void APaintableActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APaintableActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APaintableActorBase::PaintActor(const FHitResult& Hit, 
	const FLinearColor& Color, 
	bool bIsTemporary, 
	float TempPaintLifetime,
	const float ScaleModifier)
{
	UPaintableStaticMeshComponent* HitStaticMeshComp = Cast<UPaintableStaticMeshComponent>(Hit.Component);
	UPaintableSkeletalMeshComponent* HitSkeletalMeshComp = Cast<UPaintableSkeletalMeshComponent>(Hit.Component);
	if (HitStaticMeshComp)
	{
		HitStaticMeshComp->PaintMesh(Hit, Color, ScaleModifier);
	}
	else if (HitSkeletalMeshComp)
	{
		HitSkeletalMeshComp->PaintMesh(Hit, Color);
	}
}

void APaintableActorBase::ScanActor(const float ScanSpeed, const float Range, const FVector& ScanOrigin)
{
	TArray<UActorComponent*> PaintableMeshes = GetComponentsByClass(UScannableStaticMesh::StaticClass());
	UE_LOG(LogTemp, Warning, TEXT("Scanning Actor"));
	for (UActorComponent* Mesh : PaintableMeshes)
	{
		UScannableStaticMesh* ScannableMesh = Cast<UScannableStaticMesh>(Mesh);
		if (ScannableMesh != nullptr)
		{
			ScannableMesh->ScanMesh(ScanSpeed, Range, ScanOrigin);
		}
	}
}
