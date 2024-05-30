// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameModeBase.h"

#include "FPSGameInstance.h"
#include "MainMenuGameStateBase.h"
#include "PlayerInfoState.h"

void AMainMenuGameModeBase::AssignTeamToPlayer(APlayerController* NewPlayer)
{
    if (!NewPlayer)
    {
        return;
    }

    APlayerInfoState* FPSPlayerState = Cast<APlayerInfoState>(NewPlayer->PlayerState);
    if (FPSPlayerState && GS)
    {
        FPSPlayerState->SetPlayerController(Cast<APlayerController>(NewPlayer));
        GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Red, FString::Printf(TEXT("PlayerState slot location line 21:  %d"), GS->lastSlotFilled));

            if(GS->lastSlotFilled == -1)
            {
                GS->lastSlotFilled = 0;
                FPSPlayerState->SetSlotLocation(GS->lastSlotFilled);
				 FPSPlayerState->TeamId = AssignTeam(FPSPlayerState);
                 FPSPlayerState->SetConnectionState(EConnection::Lobby);
                 GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Red, FString::Printf(TEXT("PlayerState slot location line 29: %d"),GS->lastSlotFilled));

                 return;
            }

            int32 SlotKey = CalculateNextSlot(GS->lastSlotFilled);

                 FPSPlayerState->SetSlotLocation(SlotKey);
                FPSPlayerState->TeamId = AssignTeam(FPSPlayerState);
                GS->lastSlotFilled = SlotKey;
                GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Red, FString::Printf(TEXT("PlayerState slot location line 38: %d"), GS->lastSlotFilled));

    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Red, FString::Printf(TEXT("Either FPSPlayerState failed or GS failed")));
    }
}

int32 AMainMenuGameModeBase::CalculateNextSlot(int32 lastSlot)
{
    if(lastSlot < 5)
    {
        return lastSlot + 5;
    }
    return (lastSlot - 4);
}
void AMainMenuGameModeBase::BeginPlay()
{
	Super::BeginPlay();


    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = true; 
        PlayerController->bEnableClickEvents = true; 
        PlayerController->bEnableMouseOverEvents = true;
    }

}

void AMainMenuGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

    GS = Cast<AMainMenuGameStateBase>(GetWorld()->GetGameState());

    if (HasAuthority() && GS)
    {
        GS->PlayerStates.AddUnique(Cast<APlayerInfoState>(NewPlayer->PlayerState));

            
            UFPSGameInstance* GI = Cast<UFPSGameInstance>(GetGameInstance());
        if(GI)
        {
            GI->OnPlayerJoin.Broadcast();
        }
        
    }
	else
    {
        UE_LOG(LogTemp, Warning, TEXT("GS failed"));
    }
}

AMainMenuGameModeBase::AMainMenuGameModeBase()
{
    bUseSeamlessTravel = true;
}

ETeam AMainMenuGameModeBase::AssignTeam(APlayerInfoState*PlayerInfoState)
{
	//TODO:: add matchmaking type of sorting
    if (PlayerInfoState->GetSlotLocation() >= 0 && PlayerInfoState->GetSlotLocation() <= 4)
    {
        RedTeamPlayerCount++;
        return ETeam::TEAM_RED;
    }

	if (PlayerInfoState->GetSlotLocation() >= 5 && PlayerInfoState->GetSlotLocation() <= 9)
    {
        BlueTeamPlayerCount++;
        return ETeam::TEAM_BLUE;
    }

        return ETeam::TEAM_NONE;
    
}
