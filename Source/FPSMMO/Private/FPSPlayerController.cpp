// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"

#include "FPSGameState.h"
#include "MainHUD.h"
#include "PlayerHUD.h"
#include "PlayerInfoState.h"

AFPSPlayerController::AFPSPlayerController()
{

	bReplicates = true;
}


void AFPSPlayerController::SetPlayerState(APlayerInfoState* PlayerInfoState)
{
	PIS = PlayerInfoState;
}

void AFPSPlayerController::ClientUpdateRespawnTime_Implementation(float RespawnTimeLeft)
{
	APlayerHUD* PlayerHUD = Cast<APlayerHUD>(GetHUD());
	if (PlayerHUD)
	{
		PlayerHUD->UpdateRespawnTime(RespawnTimeLeft);
	}
}
void AFPSPlayerController::ClientUpdateBountyUI_Implementation(const FText& NewText)
{
	APlayerHUD* PlayerHUD = Cast<APlayerHUD>(GetHUD());
	if (PlayerHUD)
	{
		PlayerHUD->UpdateGameInfoText(NewText);
	}
}

void AFPSPlayerController::ClientUpdateScoreboard_Implementation()
{
	APlayerHUD* PlayerHUD = Cast<APlayerHUD>(GetHUD());
	if (PlayerHUD)
	{
		PlayerHUD->RefreshScoreboard();
	}
}


void AFPSPlayerController::OnHUDReady(APlayerHUD* playerHUD)
{
	if(playerHUD)
	{
		if (AFPSGameState* GS = Cast<AFPSGameState>(GetWorld()->GetGameState()))
		{
			UE_LOG(LogTemp, Warning, TEXT("OnHUDReady Called"));
			GS->OnPlayersChanged.AddDynamic(playerHUD, &APlayerHUD::RefreshScoreboard);
		}
	}
}

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

}
