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


// Sets default values
AGPlayerCharacter::AGPlayerCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UGMovementComponent>(ACharacter::CharacterMovementComponentName)),
GMovementComponent(Cast<UGMovementComponent>(GetCharacterMovement()))
{
	
	
	GMovementComponent->SetIsReplicated(true);

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
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));	
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength=600.0f;
	CameraBoom->bUsePawnControlRotation = true;
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;



	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	

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

		// Crouching
		//MOVED TO Blueprint
		// Sprinting
		//Moved to Blueprint
		// Dash
		//Moved to Blueprint
		

	
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

	UE_LOG(LogTemp, Warning, TEXT("Jump isserver:%d"), HasAuthority())
}

void AGPlayerCharacter::StopJumping()
{
	bPressedGhostJump = false;
	Super::StopJumping();
}


// CROUCH SECTION


