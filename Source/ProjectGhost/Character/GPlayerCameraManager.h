// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "GPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGHOST_API AGPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly) float CrouchBlendDuration=.9f;
	float CrouchBlendTime;

public:
	AGPlayerCameraManager();

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
};
