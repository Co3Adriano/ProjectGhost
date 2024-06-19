// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "GPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


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
	bIsCrouched = GCharacter->bIsCrouched;
	bIsAiming = GCharacter->IsAiming();
	FRotator AimRotation = GCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(GCharacter->GetVelocity());
	YawOffset  = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = GCharacter->GetActorRotation();
	const FRotator Delta =UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.0f);
	Lean = FMath::Clamp(Interp, -90.0f, 90.0f);

}
