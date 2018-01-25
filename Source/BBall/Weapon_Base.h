// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon_Base.generated.h"

USTRUCT()
struct FWeaponConfig
{
	GENERATED_BODY()

	FWeaponConfig()
	{
		AttachSocketName = "WeaponSocket";
		WeaponDisplayName = "Weapon";
		AmmoCost = 1;
		SpareClips = 4;
		ClipSize = 30;
		BulletDamage = 30.f;
		Range = 10000.f;
	}

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocketName;

	UPROPERTY(EditDefaultsOnly)
	FString WeaponDisplayName;

	UPROPERTY(EditDefaultsOnly)
	int32 SpareClips;

	UPROPERTY(EditDefaultsOnly)
	int32 ClipSize;

	UPROPERTY(EditDefaultsOnly)
	int32 AmmoCost;

	UPROPERTY(EditDefaultsOnly)
	float BulletDamage;

	UPROPERTY(EditDefaultsOnly)
	float Range;
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

	void OnEquip();
	void OnUnEquip();

	/** Start and stop firing */
	void BeginFire();
	void EndFire();

	/** Actual firing method */
	virtual void Fire();

	void SetHoldingPawn(class ABBallCharacter* character);

	USkeletalMeshComponent* GetRelevantWeaponMesh() const;
	
	
};
