// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGHOST_API AGHUD : public AHUD
{
	GENERATED_BODY()


public:
	virtual void  DrawHUD() override;
	
	
};
