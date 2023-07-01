// Copyright Epic Games, Inc. All Rights Reserved.


#include "FPSMMOGameModeBase.h"
#include "PlayerInfoState.h"

void AFPSMMOGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

    APlayerInfoState* FPSPlayerState = Cast<APlayerInfoState>(NewPlayer->PlayerState);
    if (FPSPlayerState)
    {
        FPSPlayerState->SetPlayerController(NewPlayer);
    }
}
