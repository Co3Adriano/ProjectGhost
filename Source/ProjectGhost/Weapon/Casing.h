// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"


UCLASS()
class PROJECTGHOST_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACasing();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason);

	
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CasingMesh;

	UPROPERTY(EditAnywhere)
	float CasingImpulse;

	UPROPERTY(EditAnywhere)
	class USoundCue* CasingSound;

	static int32 CasingCount;
	static TQueue<ACasing*> CasingQueue;
	bool bSoundPlayed;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void CheckBulletCasingCount();
	
	UPROPERTY(EditAnywhere)
	int32 MaxCasingCount  = 50;

	
	int32 RandomInt = FMath::RandRange(1, 5);
};
