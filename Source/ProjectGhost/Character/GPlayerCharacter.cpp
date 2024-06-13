// Fill out your copyright notice in the Description page of Project Settings.


#include "GPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AGPlayerCharacter::AGPlayerCharacter()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));	
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength=600.0f;
	FollowCamera->bUsePawnControlRotation = true;
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	

}

// Called when the game starts or when spawned
void AGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void AGPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

