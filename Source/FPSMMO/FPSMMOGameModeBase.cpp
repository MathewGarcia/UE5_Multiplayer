// Copyright Epic Games, Inc. All Rights Reserved.


#include "FPSMMOGameModeBase.h"
#include "PlayerInfoState.h"

APawn* AFPSMMOGameModeBase::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{

    APlayerInfoState* NewPlayerInfoState = Cast<APlayerInfoState>(NewPlayer->PlayerState);
    if(NewPlayerInfoState)
    {
        NewPlayerInfoState->TeamId = AssignTeam();
    }

    return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);

}

void AFPSMMOGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

    APlayerInfoState* FPSPlayerState = Cast<APlayerInfoState>(NewPlayer->PlayerState);
    if (FPSPlayerState)
    {
        FPSPlayerState->SetPlayerController(NewPlayer);
    }
}
