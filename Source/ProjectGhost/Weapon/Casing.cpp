// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

int32 ACasing::CasingCount = 0;
TQueue<ACasing*> ACasing::CasingQueue;
ACasing::ACasing()
{
 	PrimaryActorTick.bCanEverTick = false;
	
	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	CasingImpulse = 20.0f;
	bSoundPlayed = false;
	
}


void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector() * CasingImpulse * RandomInt  , NAME_None, true);

	CasingCount++;
	CasingQueue.Enqueue(this);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Casing Count: %d"), CasingCount));

	
	CheckBulletCasingCount();
	
	
	
}

void ACasing::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CasingCount--;
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("Casing Count: %d"), CasingCount));
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bSoundPlayed && CasingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CasingSound, GetActorLocation());
		bSoundPlayed = true;
	}
	
}

void ACasing::CheckBulletCasingCount()
{
	
	while (CasingCount > MaxCasingCount)
	{
		ACasing* OldestCasing;
		if (CasingQueue.Dequeue(OldestCasing))
		{
			OldestCasing->Destroy();
		}
	}
}

