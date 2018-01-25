// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon_Hitscan.h"
#include "BBallCharacter.h"
#include "BBall.h"

void AWeapon_Hitscan::Fire()
{

}

FHitResult AWeapon_Hitscan::TraceHit(FVector StartTrace, FVector EndTrace)
{
	FHitResult Hit;

	//FCollisionQueryParams QueryParams;
	//QueryParams.AddIgnoredActor(HoldingPawn);

	//GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, QueryParams);

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
