// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

#include "GPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


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

	
	
	
}
