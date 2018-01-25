// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon_Base.h"
#include "Weapon_Hitscan.generated.h"

USTRUCT()
struct FHitscanWeaponConfig
{
	GENERATED_BODY()

	FHitscanWeaponConfig()
	{
		HipFireSpread = 5.f;
		HitscanRange = 10000.f;
		HitscanDamage = 30.f;
	}

	UPROPERTY(EditDefaultsOnly, Category = "Accuracy")
	float HipFireSpread;

	UPROPERTY(EditDefaultsOnly, Category = "Range")
	float HitscanRange;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float HitscanDamage;
};

/**
 * 
 */
UCLASS()
class BBALL_API AWeapon_Hitscan : public AWeapon_Base
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Configuration")
	FHitscanWeaponConfig HitscanConfig;

	virtual void Fire() override;

	/** [server] Does a line trace and caps the result */
	FHitResult TraceHit(FVector StartTrace, FVector EndTrace);
	
	/** [server] Takes a hit result, check if we hit another player and deal damage to them if so*/
	bool ProcessHit(const FHitResult& Hit);
	
};
