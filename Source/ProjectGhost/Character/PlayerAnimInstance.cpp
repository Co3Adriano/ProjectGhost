// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "GPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectGhost/Weapon/Weapon.h"



void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	GCharacter = Cast<AGPlayerCharacter>(TryGetPawnOwner());
	
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if(GCharacter == nullptr)
	{
		GCharacter = Cast<AGPlayerCharacter>(TryGetPawnOwner());
	}

	if(GCharacter == nullptr) return;

	
	FVector Velocity = GCharacter->GetVelocity();
	Velocity.Z = 0.0f;
	Speed = Velocity.Size();
	
	
	bIsInAir = GCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = GCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;
	bWeaponEquipped = GCharacter->IsWeaponEquipped();
	EquippedWeapon = GCharacter->GetEquippedWeapon();
	bIsCrouched = GCharacter->bIsCrouched;
	bIsAiming = GCharacter->IsAiming();
	TurningInPlace = GCharacter->GetTurningInPlace();

	FRotator AimRotation = GCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(GCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;
	
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = GCharacter->GetActorRotation();
	const FRotator Delta =UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.0f);
	Lean = FMath::Clamp(Interp, -90.0f, 90.0f);
	
	AO_Yaw = GCharacter->GetAO_Yaw();
	AO_Pitch = GCharacter->GetAO_Pitch();

	AimYaw = GCharacter ->GetAimYaw();
	AimPitch = GCharacter->GetAimPitch();
	
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && GCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		GCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
	}
}
