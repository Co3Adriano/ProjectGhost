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
//	FPSCameraTransform = FTransform(GCharacter->GetBaseAimRotation(), GCharacter->FPSCamera->GetComponentLocation());
	
	// Left hand Transform for left hand position to left hand socket
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && GCharacter->GetMesh())
	{
		// Transform the LeftHandSocket location into the space of the right hand bone
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		// Update LeftHandTransform with the calculated position and rotation
		GCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		// Draw Debug Sphere at LeftHandSocket Location
		DrawDebugSphere(GetWorld(), LeftHandTransform.GetLocation(), 5.f, 12, FColor::Green, false, 0.1f);
		
		//Scope  Sockets for Transforming SocketTransform to Viewport Center	
		ScopeTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("ScopeSocket"), RTS_World);
		// Log the scope location
		UE_LOG(LogTemp, Warning, TEXT("Scope Socket Location: %s"), *ScopeTransform.GetLocation().ToString());

		// Draw Debug Sphere at ScopeSocket Location
		DrawDebugSphere(GetWorld(), ScopeTransform.GetLocation(), 5.f, 12, FColor::Red, false, 0.1f);
		
		//  Calculate Location Offset to align ScopeSocket with the Camera Viewport Center
		FVector TargetLocation = FPSCameraTransform.GetLocation() + FPSCameraTransform.GetRotation().Vector() * 100.0f; // Adjust distance as needed
		FVector LocationOffset = TargetLocation - ScopeTransform.GetLocation();

		// Draw Debug Line from ScopeSocket to TargetLocation
		//DrawDebugLine(GetWorld(), ScopeTransform.GetLocation(), TargetLocation, FColor::Blue, false, 0.1f, 0, 2.f);

		// Calculate Rotation Offset to align the ScopeSocket with the Camera's forward direction
		FRotator RotationOffset = FPSCameraTransform.GetRotation().Rotator() - ScopeTransform.GetRotation().Rotator();

		// Hand Transform
		LeftHandDistance = LeftHandTransform.GetLocation() - ScopeTransform.GetLocation();
		
		// Store the calculated offsets for use in the Animation Blueprint
		AimOffsetLocation = LocationOffset;
		AimOffsetRotation = RotationOffset;
		
		// Debugging: Output the calculated offsets
		
		// Log the calculated offsets
		//UE_LOG(LogTemp, Warning, TEXT("Aim Offset Location: %s"), *AimOffsetLocation.ToString());
		//UE_LOG(LogTemp, Warning, TEXT("Aim Offset Rotation: %s"), *AimOffsetRotation.ToString());

		// Draw Debug Arrows to visualize direction and orientation
	//	DrawDebugLine(GetWorld(), FPSCameraTransform.GetLocation(), TargetLocation, FColor::Yellow, false, 0.1f, 0, 2.f);
		DrawDebugSphere(GetWorld(), TargetLocation, 5.f, 12, FColor::Cyan, false, 0.1f);
	}
		
						
	}

	
	

