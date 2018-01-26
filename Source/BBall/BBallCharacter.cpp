// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BBallCharacter.h"
#include "Weapon_Base.h"
#include "BBallProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ABBallCharacter

ABBallCharacter::ABBallCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
    Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

    GetMesh()->SetOwnerNoSee(true);

}

void ABBallCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		Health = 100.f;
		SpawnDefaultWeapon();
	}
}

void ABBallCharacter::SpawnDefaultWeapon()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	if (DefaultWeapon)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AWeapon_Base* Weapon = GetWorld()->SpawnActor<AWeapon_Base>(DefaultWeapon, SpawnInfo);
		SetWeapon(Weapon);
	}
}

void ABBallCharacter::ServerBeginFire_Implementation()
{
	BeginFire();
}

bool ABBallCharacter::ServerBeginFire_Validate()
{
	return true;
}

void ABBallCharacter::ServerEndFire_Implementation()
{
	EndFire();
}

bool ABBallCharacter::ServerEndFire_Validate()
{
	return true;
}

void ABBallCharacter::BeginFire()
{
	if (Role < ROLE_Authority)
	{
		ServerBeginFire();
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->BeginFire();
	}
}

void ABBallCharacter::SetWeapon(class AWeapon_Base* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		CurrentWeapon = Weapon;

		CurrentWeapon->SetHoldingPawn(this);
		CurrentWeapon->OnEquip();

		OnRep_CurrentWeapon();
	}
}

void ABBallCharacter::ServerReload_Implementation()
{
	Reload();
}

bool ABBallCharacter::ServerReload_Validate()
{
	return true;
}

void ABBallCharacter::Reload()
{
	if (Role < ROLE_Authority)
	{
		ServerReload();
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}

void ABBallCharacter::OnRep_CurrentWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->SetHoldingPawn(this);
		CurrentWeapon->OnEquip();
	}
}

void ABBallCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABBallCharacter, CurrentWeapon);
	DOREPLIFETIME(ABBallCharacter, Health);
	DOREPLIFETIME(ABBallCharacter, bIsDead);

}

//////////////////////////////////////////////////////////////////////////
// Input

void ABBallCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABBallCharacter::BeginFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ABBallCharacter::EndFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABBallCharacter::Reload);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ABBallCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABBallCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

float ABBallCharacter::TakeDamage(float Damage, const FDamageEvent & DamageEvent, AController * Controller, AActor * Actor)
{
	if (Role < ROLE_Authority)
	{
		return 0.f;
	}

	if (bIsDead)
	{
		return 0.f;
	}

	Health -= Damage;

	if (Health > 0)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && HitAnimation)
		{
			AnimInstance->Montage_Play(HitAnimation);
		}
	}
	else
	{
		bIsDead = true;
	}
	return Damage;
}

void ABBallCharacter::EndFire()
{
	if (Role < ROLE_Authority)
	{
		ServerEndFire();
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->EndFire();
	}
}

void ABBallCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ABBallCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

USkeletalMeshComponent* ABBallCharacter::GetRelevantPlayerMesh() const
{
	if (IsLocallyControlled())
	{
		return Mesh1P;
	}
	else
	{
		return GetMesh();
	}
}