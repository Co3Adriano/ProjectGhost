// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGHOST_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()



public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;

	virtual void Fire(const FVector& HitTarget) override;
};
