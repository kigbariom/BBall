// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon_Base.h"
#include "BBall.h"
#include "BBallCharacter.h"


// Sets default values
AWeapon_Base::AWeapon_Base()
{

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh1P");
	Mesh1P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh1P->SetOnlyOwnerSee(true);

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh3P");
	Mesh3P->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh3P->SetOwnerNoSee(true);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

}

void AWeapon_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeapon_Base, HoldingPawn);
	DOREPLIFETIME_CONDITION(AWeapon_Base, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeapon_Base, CurrentAmmoInClip, COND_OwnerOnly);

}

USkeletalMeshComponent* AWeapon_Base::GetRelevantWeaponMesh() const
{
	if (HoldingPawn && HoldingPawn->IsLocallyControlled())
	{
		return Mesh1P;
	}
	else
	{
		return Mesh3P;
	}
}

// Called when the game starts or when spawned
void AWeapon_Base::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		CurrentAmmoInClip = WeaponConfig.ClipSize;
		CurrentAmmo = WeaponConfig.ClipSize * WeaponConfig.SpareClips;
	}
	
}

void AWeapon_Base::SetHoldingPawn(ABBallCharacter* Pawn)
{
	if (!Pawn) return;

	Instigator = Pawn;
	HoldingPawn = Pawn;

	SetOwner(HoldingPawn);
}

void AWeapon_Base::OnEquip()
{
	USkeletalMeshComponent* RelevantPlayerMesh = HoldingPawn->GetRelevantPlayerMesh();
	USkeletalMeshComponent* RelevantWeaponMesh = GetRelevantWeaponMesh();

	RelevantWeaponMesh->AttachToComponent(RelevantPlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponConfig.AttachSocketName);
}

void AWeapon_Base::OnUnEquip()
{
	USkeletalMeshComponent* RelevantWeaponMesh = GetRelevantWeaponMesh();
	RelevantWeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void AWeapon_Base::BeginFire()
{
	Fire();
	bWantsToFire = true;
}

void AWeapon_Base::EndFire()
{
	bWantsToFire = false;
}

void AWeapon_Base::Fire()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	if (CurrentAmmoInClip <= 0)
	{
		if (CurrentAmmo <= 0)
		{
			return;
		}
		Reload();
		return;
	}

	CurrentAmmoInClip--;

	MulticastPlayShootingEffects();

	GetWorldTimerManager().SetTimer(TimerHandle_RefireTimer, this, &AWeapon_Base::OnRefire, WeaponConfig.TimeBetweenShots, false);

}

void AWeapon_Base::OnRefire()
{
	if (bWantsToFire)
	{
		Fire();
	}
}

void AWeapon_Base::MulticastPlayShootingEffects_Implementation()
{
	
	if (WeaponEffects.GunshotParticles)
	{
		UGameplayStatics::SpawnEmitterAttached(WeaponEffects.GunshotParticles, GetRelevantWeaponMesh(), "Muzzle");
	}
}

void AWeapon_Base::Reload()
{
	if (Role < ROLE_Authority)
	{
		return;
	}
	if (CurrentAmmoInClip < WeaponConfig.ClipSize)
	{
		if (WeaponConfig.bReloadIndividually)
		{
			CurrentAmmo -= 1;
			CurrentAmmoInClip += 1;

			//MulticastPlayReloadFX
		}
		else
		{
			int32 AmmoPutInClip = FMath::Min(WeaponConfig.ClipSize - CurrentAmmoInClip, CurrentAmmo - CurrentAmmoInClip);
			if (AmmoPutInClip >= 0)
			{
				CurrentAmmo -= AmmoPutInClip;
				CurrentAmmoInClip += AmmoPutInClip;

				//MulticastPlayReloadFX
			}
		}
	}
	
}
