// Blending Camera for Crouching in Third Person
// 
// in BP_PlayerController set CameraManagerClass to AGPlayerCameraManager

// STILL THIS SHIT ISN'T WORKING CORRECTLY
#include "GPlayerCameraManager.h"

#include "GPlayerCharacter.h"
#include "GMovementComponent.h"
#include "Components/CapsuleComponent.h"

AGPlayerCameraManager::AGPlayerCameraManager(): CrouchBlendTime(0)
{
}

void AGPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (AGPlayerCharacter* PlayerCharacter = Cast<AGPlayerCharacter>(GetOwningPlayerController()->GetPawn()))
	{
		UGMovementComponent* PMC = PlayerCharacter->GetGPlayerCharacterMovement();
		
		FVector TargetCrouchOffset = FVector(
			0,
			0,
			PMC->GetCrouchedHalfHeight() - PlayerCharacter->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
		);
		FVector Offset = FMath::Lerp(FVector::ZeroVector, TargetCrouchOffset, FMath::Clamp(CrouchBlendTime / CrouchBlendDuration, 0.f, 1.f));

		if (PMC->IsCrouching())
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime + DeltaTime, 0.f, CrouchBlendDuration);
			Offset -= TargetCrouchOffset;
		}
		else
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime - DeltaTime, 0.f, CrouchBlendDuration);
		}

		OutVT.POV.Location += Offset;
	}
}
