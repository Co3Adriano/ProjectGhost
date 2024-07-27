// Fill out your copyright notice in the Description page of Project Settings.


#include "GPlayerCharacter.h"

#include "GMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

#include "Net/UnrealNetwork.h"
#include "ProjectGhost/Combat/CombatComponent.h"
#include "ProjectGhost/Weapon/Weapon.h"
#include "PlayerAnimInstance.h"
// Helper Macros
#if 0
float MacroDuration = 2.f;
#define SLOG(x) GEngine->AddOnScreenDebugMessage(-1, MacroDuration ? MacroDuration : -1.f, FColor::Yellow, x);
#define POINT(x, c) DrawDebugPoint(GetWorld(), x, 10, c, !MacroDuration, MacroDuration);
#define LINE(x1, x2, c) DrawDebugLine(GetWorld(), x1, x2, c, !MacroDuration, MacroDuration);
#define CAPSULE(x, c) DrawDebugCapsule(GetWorld(), x, CapHH(), CapR(), FQuat::Identity, c, !MacroDuration, MacroDuration);
#else
#define SLOG(x)
#define POINT(x, c)
#define LINE(x1, x2, c)
#define CAPSULE(x, c)
#endif
// Sets default values
AGPlayerCharacter::AGPlayerCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UGMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	GMovementComponent=Cast<UGMovementComponent>(GetCharacterMovement());
	GMovementComponent->SetIsReplicated(true);


	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
	
	PrimaryActorTick.bCanEverTick = true;

	// CAPSULE COMPONENT FROM BLUEPRINT
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;


	//Camera Section
	//Third Person Camera (Only for Debug)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength=400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetActive(false);


	//First Person Camera
	FPSCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("FPSCameraBoom"));
	
	FPSCameraBoom->TargetArmLength = 5.0f;
	FPSCameraBoom->SetupAttachment(GetMesh(),  "neck_02");
	FPSCameraBoom->bUsePawnControlRotation = true;
	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	FPSCamera->SetupAttachment(FPSCameraBoom, USpringArmComponent::SocketName);
	FPSCamera->bUsePawnControlRotation = false;
	FPSCamera->FieldOfView = 90.f;
	FPSCamera->SetActive(false);

		
	
	// Orient rotation to movement 
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;


	// Overhead Widget Section
	OverheadWidget  = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	Combat-> SetIsReplicated(true);
	
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	FPCameraYaw = 0.f;
	FPCameraPitch = 0.f;

}


void AGPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGPlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AGPlayerCharacter, LeaningAmount);
}




////////////////////////////////////////////////////////////////////
/* BEGIN AND TICK*/
void AGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
void AGPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);

	// Calculate Aim Offset every tick toDo DeltaTime as Argument
	CalculateFPCameraOrientation();
}



/* BEGIN AND TICK END*/
////////////////////////////////////////////////////////////////////



/* INPUT */
void AGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGPlayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGPlayerCharacter::Look);

		EnhancedInputComponent->BindAction(UseAction, ETriggerEvent::Triggered, this, &AGPlayerCharacter::Use);


		// Crouch Function 
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AGPlayerCharacter::CrouchButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AGPlayerCharacter::CrouchButtonReleased);
	
		// Aim Function 
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AGPlayerCharacter::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AGPlayerCharacter::AimButtonReleased);

		//Fire Function
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AGPlayerCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AGPlayerCharacter::FireButtonReleased);
	

		//Leaning Function
		EnhancedInputComponent->BindAction(LeanLeftAction, ETriggerEvent::Started, this, &AGPlayerCharacter::StartLeaningLeft);
		EnhancedInputComponent->BindAction(LeanLeftAction, ETriggerEvent::Completed, this, &AGPlayerCharacter::StopLeaningLeft);
		EnhancedInputComponent->BindAction(LeanRightAction, ETriggerEvent::Started, this, &AGPlayerCharacter::StartLeaningRight);
		EnhancedInputComponent->BindAction(LeanRightAction, ETriggerEvent::Completed, this, &AGPlayerCharacter::StopLeaningRight);
	}

}

void AGPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(Combat)
	{
		Combat->Character = this;
		
	}
	
}


/// BASE Movement ///
// Move
void AGPlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		
		
		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}
// Look
void AGPlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}




//COSTUME MOVEMENT COMPONENT SECTION
FCollisionQueryParams AGPlayerCharacter::GetIgnoreCharacterParams() const
{
	FCollisionQueryParams Params;

	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);
	Params.AddIgnoredActors(CharacterChildren);
	Params.AddIgnoredActor(this);

	return Params;
}



void AGPlayerCharacter::Jump()
{
	bPressedGhostJump = true;
	
	Super::Jump();
	
	bPressedJump = false;

	UnCrouch();
	
	

	UE_LOG(LogTemp, Warning, TEXT("Jump isserver:%d"), HasAuthority())
}

void AGPlayerCharacter::StopJumping()
{
	bPressedGhostJump = false;
	Super::StopJumping();
}




void AGPlayerCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;

	if(IsLocallyControlled())
	{
		if(OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
	
}



void AGPlayerCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if(LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
		
	}
}




void AGPlayerCharacter::Use()
{
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
			
			
		}
		else
		{
			ServerUse();

		}
		
		
		if (OverlappingWeapon)
		{
			
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Has Rifle set to True"));
		}
	}
}

void AGPlayerCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.0f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	// Turn in Place Not Working Correctly
	/*if (Speed == 0.f && !bIsInAir) // standing or crouching
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);

		AO_Yaw = DeltaAimRotation.Yaw;
		if(TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			Interp_AO_Yaw = AO_Yaw;
			
		}
		
	}*/
	if(Speed == 0.f && !bIsInAir) // standing or crouching
	{
		TurnInPlace(DeltaTime);
	}
	
	if (Speed > 0.f || bIsInAir) // running, or jumping
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		
	}
	AO_Pitch = GetBaseAimRotation().Pitch;

	if(AO_Pitch > 90.f && !IsLocallyControlled())
	{
		//Map Pitch to range to [270, 360) to [-90, 0]
		FVector2d  InRange (270.f, 360.f);
		FVector2d  OutRange (-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	
	}



	
}

void AGPlayerCharacter::FireButtonPressed()
{
	if(Combat)
	{
		Combat->FireButtonPressed(true);
		
	}
}

void AGPlayerCharacter::FireButtonReleased()
{
	if(Combat)
	{
		Combat->FireButtonPressed(false);
		
	}
}

void AGPlayerCharacter::TurnInPlace(float DeltaTime)
{

	if (AO_Yaw > 90.f)
	{	
		TurningInPlace = ETurningInPlace::ETIP_Right;
		
	}

	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	
	if(TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		Interp_AO_Yaw = FMath::FInterpTo(Interp_AO_Yaw, 0.f, DeltaTime, 5.f);
		AO_Yaw = Interp_AO_Yaw;

		if(FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}


void AGPlayerCharacter::ServerUse_Implementation()
{
	if (Combat)
	{

		Combat->EquipWeapon(OverlappingWeapon);

		
		if (OverlappingWeapon)
		{
			
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Has Rifle set to True"));
		}
	}

}

 
bool AGPlayerCharacter::IsWeaponEquipped()
{
	return(Combat && Combat->EquippedWeapon);
}


// CROUCH SECTION coming from GMovementComponent

void AGPlayerCharacter::CrouchButtonPressed()
{
	if(GMovementComponent)
	{
		GMovementComponent->CrouchPressed();
	}
	else
	{
		SLOG("GMovementComponent is NULL");
	}
	
}

void AGPlayerCharacter::CrouchButtonReleased()
{
	if(GMovementComponent)
	{
		GMovementComponent->CrouchReleased();
	}
	else
	{
		SLOG("GMovementComponent is NULL");
	}
	
}

void AGPlayerCharacter::AimButtonPressed()
{
	if(Combat)
	{
		Combat->SetAiming(true);
	}
}
void AGPlayerCharacter::AimButtonReleased()
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}


bool AGPlayerCharacter::IsAiming() const
{
	return (Combat && Combat->bAiming);
}

AWeapon* AGPlayerCharacter::GetEquippedWeapon() const
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

void AGPlayerCharacter::PlayFireMontage(bool bAiming)
{
	if(Combat==nullptr|| Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage, 1.f);
		FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(FName(SectionName), FireWeaponMontage);
	}
	
	
}


// LEANING AMOUNT VALUES


void AGPlayerCharacter::StartLeaningLeft()
{
	LeaningAmount = -45;
	if(!HasAuthority())
	{
		ServerStartLeaningLeft();
	}
}

void AGPlayerCharacter::StopLeaningLeft()
{
	LeaningAmount = 0;
	if(!HasAuthority())
	{
		ServerStopLeaningLeft();
	}
}

void AGPlayerCharacter::StartLeaningRight()
{
	LeaningAmount = 45;
	if(!HasAuthority())
	{
		ServerStartLeaningRight();
	}
}

void AGPlayerCharacter::StopLeaningRight()
{
	LeaningAmount = 0;
	if(!HasAuthority())
	{
		ServerStopLeaningRight();
	}
}

void AGPlayerCharacter::ServerStartLeaningLeft_Implementation()
{
	StartLeaningLeft();
}

void AGPlayerCharacter::ServerStopLeaningLeft_Implementation()
{
	StopLeaningLeft();
}
void AGPlayerCharacter::ServerStartLeaningRight_Implementation()
{
	StartLeaningRight();
}
void AGPlayerCharacter::ServerStopLeaningRight_Implementation()
{
	StopLeaningRight();
}

//  AIM OFF SET HEAD SECTION
// Camera PITCH AND YAW  =!= Weapon Orientation 

void AGPlayerCharacter::CalculateFPCameraOrientation()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		// Get the Player Camera Manager
		APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
		if (CameraManager)
		{
			// Get Camera Rotation
			FRotator CameraRotation = CameraManager->GetCameraRotation();
			
			// Get Yaw and Pitch from Camera Rotation
			FPCameraYaw = CameraRotation.Yaw;
			// Pitch value as Client gets transformed for lower bandwidth Server Pitch =! Client Pitch
			FPCameraPitch = CameraRotation.Pitch;
			if(FPCameraPitch > 90.f && !IsLocallyControlled())
			{
				//Map Pitch to range to [270, 360) to [-90, 0]
				FVector2d  InRange (270.f, 360.f);
				FVector2d  OutRange (-90.f, 0.f);
				FPCameraPitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, FPCameraPitch);
	
			}
						
				UE_LOG(LogTemp, Warning, TEXT("Camera Pitch: %f"), FPCameraPitch);

			
				
    	
		}
	}
}

