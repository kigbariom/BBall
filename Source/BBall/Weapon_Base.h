// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon_Base.generated.h"

USTRUCT()
struct FWeaponFX
{
	GENERATED_BODY()

	/** Particle FX, bullet holes etc.*/
	FWeaponFX()
	{

	}

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* GunshotParticles;
};

USTRUCT()
struct FWeaponConfig
{
	GENERATED_BODY()

	FWeaponConfig()
	{
		AttachSocketName = "WeaponSocket";
		WeaponDisplayName = "Weapon";
		AmmoCost = 1;
		SpareClips = 5;
		ClipSize = 4;
		EquipTime = 0.2f;
		HolsterTime = 0.1f;
		ReloadTime = 2.0f;
		TimeBetweenShots = 0.8f;
		BaseFOV = 90;
		bReloadIndividually = true;
	}

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocketName;

	UPROPERTY(EditDefaultsOnly)
	FString WeaponDisplayName;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 SpareClips;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 ClipSize;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 AmmoCost;

	UPROPERTY(EditDefaultsOnly, Category = "Timing")
	float EquipTime;

	UPROPERTY(EditDefaultsOnly, Category = "Timing")
	float HolsterTime;

	UPROPERTY(EditDefaultsOnly, Category = "Timing")
	float ReloadTime;

	UPROPERTY(EditDefaultsOnly, Category = "Timing")
	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = "Timing")
	bool bReloadIndividually;

	UPROPERTY(EditDefaultsOnly, Category = "FOV")
	int32 BaseFOV;
	
};

UCLASS()
class BBALL_API AWeapon_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon_Base();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Configuration")
	FWeaponConfig WeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Configuration")
	FWeaponFX WeaponEffects;

protected:

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh3P;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** This is the pawn that is holding the gun */
	UPROPERTY(Replicated)
	class ABBallCharacter* HoldingPawn;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayShootingEffects();

public:

	UPROPERTY(Replicated)
	int32 CurrentAmmo;

	UPROPERTY(Replicated)
	int32 CurrentAmmoInClip;

	void Reload();

	void OnEquip();
	void OnUnEquip();

	/** Start and stop firing */
	void BeginFire();
	void EndFire();

	/** Actual firing method */
	virtual void Fire();

	bool bWantsToFire;

	void OnRefire();

	FTimerHandle TimerHandle_RefireTimer;

	void SetHoldingPawn(class ABBallCharacter* character);

	USkeletalMeshComponent* GetRelevantWeaponMesh() const;

public:

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FString GetWeaponDisplayName() const { return WeaponConfig.WeaponDisplayName; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetAmmoInClip() const { return CurrentAmmoInClip; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurrentAmmo() const { return CurrentAmmo; }
	
};
