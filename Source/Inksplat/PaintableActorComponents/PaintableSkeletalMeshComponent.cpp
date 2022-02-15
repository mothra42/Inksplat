// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableSkeletalMeshComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "../CustomStaticMethods/SkeletalMeshPaintingLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/Canvas.h"

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
	//UGameplayStatics::FindCollisionUV(Hit, 0, UVPosition);
	USkeletalMeshPaintingLibrary::FindCollisionUVFromHit(Hit, UVPosition);


	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, PaintTexture, Canvas, CanvasSize, Context);

	//TODO make this work for textures as well. Will probably need to include one in the constructor.
	//probably refactor this to exist in its own function
	FVector2D TileItemPosition = CanvasSize * UVPosition;
	FCanvasTileItem RectItem(
		TileItemPosition,
		FVector2D(25.f, 25.f), //size
		Color
	);

	Canvas->DrawItem(RectItem);
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
	return true;
}