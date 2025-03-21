// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Animation/AnimInstance.h"
#include "ProjectGhost/DataTypes/TurningInPlace.h"
#include "ProjectGhost/Weapon/Weapon.h"
#include "PlayerAnimInstance.generated.h"


/**
 * 
 */
UCLASS()
class PROJECTGHOST_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()



public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
	UPROPERTY(BlueprintReadOnly, Category  = "Character", meta = (AllowPrivateAccess = "true") )
	class AGPlayerCharacter* GCharacter;
	
	UPROPERTY(BlueprintReadOnly, Category  = "Character", meta = (AllowPrivateAccess = "true") )
	class USkeletalMeshComponent* Mesh;


private:
	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	float Direction;

	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	bool bWeaponEquipped;

	class AWeapon* EquippedWeapon;
	
	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	bool bIsCrouched;
	
	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	bool bIsAiming;

	
	
	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	float YawOffset;
	
	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	float Lean;

	
	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
	FRotator DeltaRotation;


	//AimOffset Yaw and Pitch
	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	float AO_Yaw;
	
	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	float AO_Pitch;

	//Camera Pitch and Yaw
	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	float FPCameraYaw;

	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	float FPCameraPitch;

	
	UPROPERTY(BlueprintReadWrite, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	FTransform LeftHandTransform;

	
	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	ETurningInPlace TurningInPlace;
	
	UPROPERTY(BlueprintReadOnly, Category  = "Movement", meta = (AllowPrivateAccess = "true") )
	float LeaningAmount;

	UPROPERTY(BlueprintReadOnly, Category = "WeaponTransform", meta = (AllowPrivateAccess = "true"))
	FTransform ScopeTransform;

	UPROPERTY(BlueprintReadOnly, Category = "WeaponTransform", meta = (AllowPrivateAccess = "true"))
	FVector LeftHandDistance;



	

	//Reference to current Weapon no CURRENT WEAPON EXIST IN COMBAT CLASS
	// for now use EquippedWeapon
	/// for later: after implementing more weapons they're should be having own IK PROPERTIES
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	class AWeapon* CurrentWeapon;


	
	// IK Properties for Aiming
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	FIKProperties IKProperties;

	//SPECIFIC CAMERA TRANSFORM
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "IK Camera", meta = (AllowPrivateAccess = "true"))
	FTransform FPSCameraTransform;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "IK Camera", meta = (AllowPrivateAccess = "true"))
	FTransform FPSRelativeCameraTransform;


	// Aim Offset Location and Rotation for Aim Down Sights
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	FVector AimOffsetLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
	FRotator AimOffsetRotation;
};
