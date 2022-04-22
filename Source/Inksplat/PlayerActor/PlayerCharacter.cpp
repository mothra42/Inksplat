// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayerCharacter.h"
#include "../PaintableGeometry/PaintableComponents/PaintableSkeletalMeshComponent.h"
#include "../Weapons/PaintGun.h"
#include "Materials/Material.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// APlayerCharacter

APlayerCharacter::APlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	FullBodyMesh = CreateDefaultSubobject<UPaintableSkeletalMeshComponent>(TEXT("FullBodyMesh"));
	FullBodyMesh->SetOwnerNoSee(true);
	FullBodyMesh->SetupAttachment(RootComponent);
	//TODO replace magic number with variable
	FullBodyMesh->SetMaxPaintedTiles(300);
	FullBodyMesh->SetIsReplicated(true);

	// Create a gun mesh component
	//FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	//FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	//FP_Gun->bCastDynamicShadow = false;
	//FP_Gun->CastShadow = false;
	//// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	//FP_Gun->SetupAttachment(RootComponent);
	//FP_Gun->bOnlyOwnerSee = true;

	//FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	//FP_MuzzleLocation->SetupAttachment(FP_Gun);
	//FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	//Painting setup
	//PaintableObjectComponent = CreateDefaultSubobject<UPaintableObjectComponent>(TEXT("PaintableObjectComponent"));
	//static ConstructorHelpers::FObjectFinder<UMaterial> PaintableMaterialObjectFinder(
	//	TEXT("/Game/PaintApplicationFactory/Materials/M_PaintableMaterial_Skeletal")
	//);
	//PaintableMaterialParent = PaintableMaterialObjectFinder.Object;
}

void APlayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	if (PaintGunClass != nullptr && GetLocalRole() == ROLE_Authority)
	{
		PlayerPaintGun = GetWorld()->SpawnActor<APaintGun>(PaintGunClass);
		PlayerPaintGun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
		PlayerPaintGun->SetOwningPlayer(this);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerCharacter::OnFireStopped);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, PlayerPaintGun);
}

void APlayerCharacter::OnFire()
{
	if (PlayerPaintGun != nullptr)
	{
		PlayerPaintGun->FireWeapon();
	}
}

void APlayerCharacter::OnFireStopped()
{
	if (PlayerPaintGun != nullptr)
	{
		PlayerPaintGun->StopFiringWeapon();
	}
}

void APlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void APlayerCharacter::PaintActor(const FHitResult& Hit, const FLinearColor& Color)
{
	FullBodyMesh->PaintMesh(Hit, Color);
}

void APlayerCharacter::OnHealthUpdate(float PercentagePainted)
{
	UE_LOG(LogTemp, Warning, TEXT("Player %s, has percentage painted %f"), *GetName(), PercentagePainted);
	if (PercentagePainted >= 1.0)
	{	
		if (GetLocalRole() == ROLE_Authority)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s has been destroyed"), *GetName());
			Destroy();
		}
	}
}

void APlayerCharacter::OnRep_PlayerPaintGun()
{
	PlayerPaintGun->SetOwningPlayer(this);
}

void APlayerCharacter::EndPlay()
{
	//TODO
	//consider making a observation camera mode on death.
	//add neat vfx maybe.
	GetController()->Destroy();
	Destroy();
}