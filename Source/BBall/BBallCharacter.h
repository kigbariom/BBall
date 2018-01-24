// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "BBallCharacter.generated.h"


class UInputComponent;

UCLASS(config=Game)
class ABBallCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

public:
	ABBallCharacter();

protected:
	virtual void BeginPlay();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	//UFUNCTION(Server, Reliable, WithValidation)
	//void ServerBeginFire();

	void BeginFire();

	//UFUNCTION(Server, Reliable, WithValidation)
	//void ServerEndFire();

	void EndFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/** Gun related stuff*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AWeapon_Base> DefaultWeapon;

	/** Spawns the player default weapon*/
	void SpawnDefaultWeapon();

	/** [server] Sets the players weapon*/
	void SetWeapon(class AWeapon_Base* Weapon);

	/** The player's current weapon*/
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
	class AWeapon_Base* CurrentWeapon;

	/** On Rep Functions */
	UFUNCTION()
	void OnRep_CurrentWeapon();



public:
	USkeletalMeshComponent* GetRelevantPlayerMesh() const;

	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

