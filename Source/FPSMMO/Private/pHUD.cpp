// Fill out your copyright notice in the Description page of Project Settings.


#include "pHUD.h"
#include "FPSCharacter.h"
#include "PlayerInfoState.h"
#include "Net/UnrealNetwork.h"


UpHUD::UpHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UpHUD::OnRep_UpdateShield(float Shield)
{
	SP = Shield;
}

void UpHUD::OnRep_UpdateHealth(float Health)
{
			HP = Health;
}

APlayerInfoState* UpHUD::GetPlayerInfoState()
{
	if(player)
	{
		return Cast<APlayerInfoState>(player->GetPlayerState());
	}
	return nullptr;
}

FText UpHUD::UpdateText()
{
	return text;
}

void UpHUD::SetText(FText newText)
{
	text = newText;
}

void UpHUD::NativeConstruct()
{
	Super::NativeConstruct();

	player = Cast<AFPSCharacter>(GetOwningPlayerPawn());

	if(player)
	{
		OnRep_UpdateShield(player->GetShield());
		OnRep_UpdateHealth(player->GetHealth());
	}

}

void UpHUD::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate variables to all clients
	DOREPLIFETIME(UpHUD, HP);
	DOREPLIFETIME(UpHUD, SP);
}
