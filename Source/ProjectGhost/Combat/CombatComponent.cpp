// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "ProjectGhost/Character/GPlayerCharacter.h"
#include "ProjectGhost/Weapon/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"

UCombatComponent::UCombatComponent()
{
	
	
	
	PrimaryComponentTick.bCanEverTick = false;

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
	const USkeletalMeshSocket* HandSocket =  Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon,Character->GetMesh());
		if (Character == nullptr) UE_LOG(LogTemp, Warning, TEXT("Character is null"));
		if (WeaponToEquip == nullptr) UE_LOG(LogTemp, Warning, TEXT("WeaponToEquip is null"));
	}
	EquippedWeapon->SetOwner(Character);

	// issue for client facing direction not updated on the server side  (on client everything is fine) facing old Direction without moving in the corresponding direction
	// OnRep_EquippedWeapon doenst replicate facing direction on the server side
	// EquipWeapon works wonly locally on the server side so it should be fine

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

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming	? AimWalkSpeed : BaseWalkSpeed ;
		
	}
	
}
