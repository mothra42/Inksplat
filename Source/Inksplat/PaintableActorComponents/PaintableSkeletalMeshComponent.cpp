// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintableSkeletalMeshComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "../CustomStaticMethods/SkeletalMeshPaintingLibrary.h"
#include "../PlayerActor/PlayerCharacter.h"
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

	PaintCoverageArray.Init(0, 2048);
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
	DOREPLIFETIME(UPaintableSkeletalMeshComponent, NumPaintedTiles);

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
		float PaintCoverage = CalculatePaintCoverage(TileItemPosition);
	}

	FCanvasTileItem RectItem(
		TileItemPosition,
		FVector2D(22.62, 22.62), //size
		Color
	);

	Canvas->DrawItem(RectItem);
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
	return true;
}

//TODO Consider adding a fourth argument that is a reference to a reference texture to check for the bounds of the UV map
float UPaintableSkeletalMeshComponent::CalculatePaintCoverage(FVector2D Origin)
{
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
