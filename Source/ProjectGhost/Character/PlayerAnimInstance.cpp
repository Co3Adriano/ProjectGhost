// Project Ghost PlayerAnimInstance.cpp Adriano Menconi 2024 

#include "PlayerAnimInstance.h"
#include "GPlayerCharacter.h"
#include "KismetAnimationLibrary.h"
#include "Camera/CameraComponent.h"
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

	// Direction
	FRotator BaseRotation = GCharacter->GetActorRotation();
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, BaseRotation);
	
	
	
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = GCharacter->GetActorRotation();
	const FRotator Delta =UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.0f);
	Lean = FMath::Clamp(Interp, -90.0f, 90.0f);

	//aim offset yaw and pitch	
	AO_Yaw = GCharacter->GetAO_Yaw();
	AO_Pitch = GCharacter->GetAO_Pitch();

	// First Person Camera Pitch and yaw from GPlayerCharacter
	FPCameraYaw = GCharacter ->GetFPCameraYaw();
	FPCameraPitch = GCharacter->GetFPCameraPitch();
	LeaningAmount = GCharacter->GetLeaningAmount();
	
	//GET FPS CAMERA TRANSFORM for AIM OFFSET IN Control RIG 
	FPSCameraTransform = FTransform(GCharacter->GetBaseAimRotation(), GCharacter->FPSCamera->GetComponentLocation());
	
	// Left hand Transform for left hand position to left hand socket
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && GCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		GCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		//Iron Sight Sockets and Scope Sight Sockets for Transforming SocketTransform to Viewport Center	
		IronSightTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("IronSightSocket"), RTS_World);
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, IronSightTransform.GetLocation().ToString());
		
		
						
	}
	
}
