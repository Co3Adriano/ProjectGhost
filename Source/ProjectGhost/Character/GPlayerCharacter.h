// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "ProjectGhost/ProjectGhost.h"
#include "CoreMinimal.h"
#include "CableComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "GPlayerCharacter.generated.h"


class UInputMappingContext;
class UInputAction;
struct FInputActionValue;


UCLASS()
class PROJECTGHOST_API AGPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** JumpInput Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	// Custom Movement Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UGMovementComponent* GMovementComponent;


public:

	
	AGPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;

	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	virtual void BeginPlay() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	

	//CUSTOM MOVEMENT SECTION
public:
	bool bPressedGhostJump;
	

	virtual void Jump() override;
	virtual void StopJumping() override;

public:
	/*Camera Section*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UFUNCTION(BlueprintPure)
	FORCEINLINE UGMovementComponent* GetGPlayerCharacterMovement() const { return GMovementComponent; }


	FCollisionQueryParams GetIgnoreCharacterParams() const;

};
