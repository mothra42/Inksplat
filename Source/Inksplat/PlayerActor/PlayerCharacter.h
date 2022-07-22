// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interfaces/PaintableObjectInterface.h"
#include "PlayerCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UMaterial;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class APlayerCharacter : public ACharacter, public IPaintableObjectInterface
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UPaintableSkeletalMeshComponent* FullBodyMesh;

	/** Gun mesh: 1st person view (seen only by self) */
	//UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	//USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Gun class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<class APaintGun> PaintGunClass;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerPaintGun)
	APaintGun* PlayerPaintGun;


	//Abilities
	UPROPERTY(EditAnywhere, Category = Abilities)
	TSubclassOf<class UBaseAbilityComponent> PrimaryAbilityComponentClass;

	UPROPERTY(replicated)
	UBaseAbilityComponent* PrimaryAbilityComponent;

	UPROPERTY(EditAnywhere, Category = Abilities)
	TSubclassOf<class UBaseAbilityComponent> SecondaryAbilityComponentClass;

	UPROPERTY(replicated)
	UBaseAbilityComponent* SecondaryAbilityComponent;

	//Paint Material
	UMaterial* PaintableMaterialParent;

	FName TeamName = "";

	UPROPERTY(ReplicatedUsing = OnRep_TeamPaintColor)
	FColor TeamPaintColor = FColor(1, 0, 0);

	UPROPERTY(Replicated, VisibleAnywhere)
	FColor TeamTempPaintColor = FColor(0, 1, 0);

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

//Methods related to equipping weapons
protected:
	UFUNCTION()
	void OnRep_PlayerPaintGun();

	UFUNCTION()
	void OnRep_TeamPaintColor();

	//UFUNCTION()
	//void OnRep_TeamTempPaintColor();


//Methods related to firing
protected:
	/** Fires a projectile. */
	void OnFire();

	void OnFireStopped();

//Ability Methods
protected:
	void UsePrimaryAbility();

	void UseSecondaryAbility();

//Movement Methods
protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	//method used to end play for player
	void EndPlay();

public:
	virtual void PaintActor(const FHitResult& Hit, const FLinearColor& Color, bool bIsTemporary = false, float TempPaintLifetime = 0.f) override;


	//Called from paintable mesh whenever it is painted. Paintable mesh class holds all info related to painting coverage
	virtual void OnHealthUpdate(float PercentagePainted);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	UPaintableSkeletalMeshComponent* GetFullBodyMesh() const { return FullBodyMesh; }

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	FName GetTeamName() const { return TeamName; }
	void SetTeamName(const FName TeamNameToSet) { TeamName = TeamNameToSet; }

	//Getter and Setter methods for paint color
	FColor GetPaintColor() const { return TeamPaintColor; }
	FColor GetTempPaintColor() const { return TeamTempPaintColor; }
	
	void SetTeamPaintColor(const FColor ColorToSet) { TeamPaintColor = ColorToSet; }
	void SetTeamTempPaintColor(const FColor ColorToSet) { TeamTempPaintColor = ColorToSet; }

};

