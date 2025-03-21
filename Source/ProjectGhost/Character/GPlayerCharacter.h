// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "ProjectGhost/ProjectGhost.h"
#include "CoreMinimal.h"
#include "CableComponent.h"
#include "EnhancedInputComponent.h"
#include "ProjectGhost/DataTypes/TurningInPlace.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "GPlayerCharacter.generated.h"


class UInputMappingContext;
class UInputAction;
struct FInputActionValue;


UCLASS()
class PROJECTGHOST_API AGPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** JumpInput Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Crouch Action Input*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	
	/** Use Action Input*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;


	// Custom Movement Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UGMovementComponent* GMovementComponent;


	// Leaning Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LeanLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LeanRightAction;

public:
		
	AGPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;
protected:
	// BASE MOVEMENT SECTION
	virtual void BeginPlay() override;
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);


	// USE SECTION
	UFUNCTION(Server, Reliable)
	void ServerUse();
	void Use();


//AIM OFFSET SECTION

	void AimOffset(float DeltaTime);
	
//FIRE SECTION

	void FireButtonPressed();
	void FireButtonReleased();
	UPROPERTY(EditAnywhere, Category = "Combat")
		class UAnimMontage* FireWeaponMontage;
	
	//CUSTOM MOVEMENT SECTION
public:
	bool bPressedGhostJump;
	

	virtual void Jump() override;
	virtual void StopJumping() override;


	//CUSTOM MOVEMENT SECTION
	/*Camera Section*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


	/*FPSCamera Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* FPSCameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FPSCamera;
	*/
	UFUNCTION(BlueprintPure)
	FORCEINLINE UGMovementComponent* GetGPlayerCharacterMovement() const { return GMovementComponent; }


	FCollisionQueryParams GetIgnoreCharacterParams() const;

private:
	/*Widget Section*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming", meta = (AllowPrivateAccess = "true"))
	FVector AimOffsetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aiming", meta = (AllowPrivateAccess = "true"))
	FRotator AimOffsetRotation;

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	float AO_Yaw;
	float Interp_AO_Yaw;
	float AO_Pitch;

	float FPCameraYaw;
	float FPCameraPitch;
	

	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	void TurnInPlace(float DeltaTime);
	
public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	void CrouchButtonPressed();
	void CrouchButtonReleased();
	void AimButtonPressed();
	void AimButtonReleased();
	bool IsAiming() const;
	AWeapon* GetEquippedWeapon() const;
	void PlayFireMontage(bool bAiming);
	void StartLeaningLeft();
	void StopLeaningLeft();
	void StartLeaningRight();
	void StopLeaningRight();
	UFUNCTION(Server,Reliable)
	void ServerStartLeaningLeft();
	UFUNCTION(Server,Reliable)
	void ServerStopLeaningLeft();
	UFUNCTION(Server,Reliable)
	void ServerStartLeaningRight();
	UFUNCTION(Server,Reliable)
	void ServerStopLeaningRight();
	
	void CalculateFPCameraOrientation();


	
	/*Leaning Section*/	
	UPROPERTY(Replicated)
	int8 LeaningAmount;
	
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }

	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

	FORCEINLINE float GetLeaningAmount() const { return LeaningAmount; }
	
	FORCEINLINE float GetFPCameraYaw() const { return FPCameraYaw; }

	FORCEINLINE float GetFPCameraPitch() const { return FPCameraPitch; }



	
	
	//Getter for TurningInPlace
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
};
