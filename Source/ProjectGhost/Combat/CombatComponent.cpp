// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGhost/Character/GPlayerCharacter.h"
#include "ProjectGhost/Weapon/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

UCombatComponent::UCombatComponent()
{
	
	
	
	PrimaryComponentTick.bCanEverTick = true;
	BaseWalkSpeed = 400.f;
	AimWalkSpeed = 250.f;
	
}



void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCombatComponent,EquippedWeapon);
	
	DOREPLIFETIME(UCombatComponent, bAiming);


}




void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	
	if (Character == nullptr||WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	//Attach Weapon to Skeletal Mesh->RightHandSocket
	const USkeletalMeshSocket* RightHandSocket =  Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	
	
	if (RightHandSocket)
	{
		RightHandSocket->AttachActor(EquippedWeapon,Character->GetMesh());
		
	//DEBUG
		if (Character == nullptr) UE_LOG(LogTemp, Warning, TEXT("Character is null"));
		if (WeaponToEquip == nullptr) UE_LOG(LogTemp, Warning, TEXT("WeaponToEquip is null"));
	}
	EquippedWeapon->SetOwner(Character);

	
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;

	//Character->bUseControllerRotationYaw = false;
	
	
	
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;

		
		//Character->bUseControllerRotationYaw = false;
		
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	
	ServerSetAiming(bIsAiming);

	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming	? AimWalkSpeed : BaseWalkSpeed ;
		
	}
	
	

}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if(bFireButtonPressed)
	{
		FHitResult HitResult;
		TraceUnderCrosshair(HitResult);
		ServerFire(HitResult.ImpactPoint);
		
	}
	
}

void UCombatComponent::TraceUnderCrosshair(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	
	if (GEngine && GEngine->GameViewport)
	{
		//Get the size of the viewport
		
		GEngine -> GameViewport -> GetViewportSize(ViewportSize);
	}
	
		
	
		
	
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld =
		UGameplayStatics::DeprojectScreenToWorld(
				UGameplayStatics::GetPlayerController(this, 0),
				CrosshairLocation,
				CrosshairWorldPosition,
				CrosshairWorldDirection
			);
	
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		FVector End = Start + CrosshairWorldDirection*TRACE_LENGTH;
		
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility
			);
	
	}
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize TraceHitTarget)
{
	if(EquippedWeapon == nullptr) return;
	if(Character)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}



void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
	
}	
void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming	? AimWalkSpeed : BaseWalkSpeed ;
		
	}
	
}
