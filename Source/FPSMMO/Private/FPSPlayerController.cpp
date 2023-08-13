// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"

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


void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

}
