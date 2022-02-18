// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableSkeletalMeshComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "../CustomStaticMethods/SkeletalMeshPaintingLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Containers/Queue.h"

UPaintableSkeletalMeshComponent::UPaintableSkeletalMeshComponent()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> PaintableMaterialFinder(
		TEXT("/Game/PaintableMaterials/M_PaintableMaterial")
	);
	if (PaintableMaterialFinder.Succeeded())
	{
		ParentMaterial = PaintableMaterialFinder.Object;
	}
}

void UPaintableSkeletalMeshComponent::BeginPlay()
{
	Super::BeginPlay();
	PaintCoverageArray.Init(0, 1048576);
	FLinearColor ClearColor = FLinearColor(0.0, 0.0, 0.0, 0.0);
	PaintTexture = UKismetRenderingLibrary::CreateRenderTarget2D(
		GetWorld(),
		1024,
		1024,
		RTF_RGBA16f,
		ClearColor
	);

	MeshMaterialInstance = UMaterialInstanceDynamic::Create(
		ParentMaterial,
		this
	);

	MeshMaterialInstance->SetTextureParameterValue(FName("ColorMap"), PaintTexture);
	SetMaterial(0, MeshMaterialInstance);
}

void UPaintableSkeletalMeshComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

bool UPaintableSkeletalMeshComponent::PaintMesh(const FHitResult& Hit, const FLinearColor& Color)
{

	UCanvas* Canvas;
	FVector2D CanvasSize;
	FDrawToRenderTargetContext Context;

	FVector2D UVPosition;
	USkeletalMeshPaintingLibrary::FindCollisionUVFromHit(Hit, UVPosition);

	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, PaintTexture, Canvas, CanvasSize, Context);

	//TODO make this work for textures as well. Will probably need to include one in the constructor.
	//probably refactor this to exist in its own function
	FVector2D TileItemPosition = CanvasSize * UVPosition;
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		//HealthCheckQueue.Enqueue(FHealthCheckInstructions(TileItemPosition, 25, 25));
		//ConsumeHealthCheckQueue();
		CalculateHealth(TileItemPosition, 25, 25);
	}
	FCanvasTileItem RectItem(
		TileItemPosition,
		FVector2D(25.f, 25.f), //size
		Color
	);

	Canvas->DrawItem(RectItem);
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
	return true;
}

void UPaintableSkeletalMeshComponent::ConsumeHealthCheckQueue()
{
	if (!bIsConsumingHealthCheck)
	{
		bIsConsumingHealthCheck = true;
		while (!HealthCheckQueue.IsEmpty())
		{
			FHealthCheckInstructions CurrentInstructions;
			if (HealthCheckQueue.Dequeue(CurrentInstructions))
			{
				CalculateHealth(CurrentInstructions.Origin, CurrentInstructions.UBounds, CurrentInstructions.VBounds);
			}
		}

		bIsConsumingHealthCheck = false;
	}
}

//TODO Consider adding a fourth argument that is a reference to a reference texture to check for the bounds of the UV map
float UPaintableSkeletalMeshComponent::CalculateHealth(FVector2D Origin, int32 U_Bounds, int32 V_Bounds)
{
	UE_LOG(LogTemp, Warning, TEXT("Starting calculate"));
	int U = Origin.X - (U_Bounds/2);
	int UEnd = Origin.X + (U_Bounds / 2);
	int V = Origin.Y - (V_Bounds/2);
	int VEnd = Origin.Y + (V_Bounds / 2);
	int32 NewColoredPixelCount = 0;
	while (U <= UEnd)
	{
		while (V <= VEnd)
		{
			if (U < 0 || V < 0 || U > 1024 || V > 1024)
			{
				V++;
				continue;
			}
			else if (PaintCoverageArray[U + V * 1024] == 0)
			{
				NewColoredPixelCount++;
				UE_LOG(LogTemp, Warning, TEXT("NewColor Pixel Count is %i"), NewColoredPixelCount);
				PaintCoverageArray[U + V * 1024] = 1;
			}

			V++;
		}
		V = Origin.Y - (V_Bounds / 2);
		U++;
	}

	//UE_LOG(LogTemp, Warning, TEXT("New colored pixels should be around 625 and is %i"), NewColoredPixelCount);
	UE_LOG(LogTemp, Warning, TEXT("Finishing Calculate"));
	return 0.0f;
}

