// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"


void UOverheadWidget::SetDisplayText(FString TextToDisplay) 
{

	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));

	}
}


void UOverheadWidget::ShowPlayerNetRole( APawn* InPawn) 
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;

	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;

	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;

	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	default: ;
	}
	FString RemoteRoleString = FString::Printf(TEXT("Remote Role : %s"), *Role);
	SetDisplayText(RemoteRoleString);

}



void UOverheadWidget::ShowPlayerName(APawn* InPawn)
{
	// Show Player Name is not Valid 
	FString Name = InPawn->GetPlayerState<APlayerState>()->GetPlayerName();

	
	// Display player name
	FString PlayerName = FString::Printf(TEXT("%s"), *Name);
	SetDisplayText(PlayerName);
	
	
}


void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();
	Super::NativeDestruct();
}