// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon_Hitscan.h"
#include "BBallCharacter.h"
#include "BBall.h"

void AWeapon_Hitscan::Fire()
{
	if (Role == ROLE_Authority)
	{
		Super::Fire();

		//Still calling Fire in child class if out of ammo, look in to that!!

		UCameraComponent* PlayerCamera = HoldingPawn->GetFirstPersonCameraComponent();

		FRandomStream RandStream(FMath::Rand());

		float ConeHalfAngle = FMath::DegreesToRadians(HitscanConfig.HipFireSpread * 0.5f);

		FVector StartTrace = PlayerCamera->GetComponentLocation();
		FVector ShootDir = RandStream.VRandCone(PlayerCamera->GetForwardVector(), ConeHalfAngle, ConeHalfAngle);
		FVector EndTrace = (ShootDir * HitscanConfig.HitscanRange) + StartTrace;

		FHitResult Hit = TraceHit(StartTrace, EndTrace);

		bool bHitEnemy = ProcessHit(Hit);

		if (bHitEnemy)
		{
			HoldingPawn->ClientShowHitmarker();
			//show hitmarker
			//play hitsound
		}
	}
}

FHitResult AWeapon_Hitscan::TraceHit(FVector StartTrace, FVector EndTrace)
{
	FHitResult Hit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(HoldingPawn);

	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, QueryParams);

	DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, false, 5.f);

	return Hit;
}

bool AWeapon_Hitscan::ProcessHit(const FHitResult& Hit)
{
	if (Hit.GetActor() && HoldingPawn)
	{
		FDamageEvent DamageEvent; 
		float DamageDealt = Hit.GetActor()->TakeDamage(HitscanConfig.HitscanDamage, DamageEvent, HoldingPawn->GetController(), this);

		ABBallCharacter* HitEnemy = Cast<ABBallCharacter>(Hit.GetActor());

		if (HitEnemy)
		{
			return true;
		}
		else return false;
	}
	return false;
}
