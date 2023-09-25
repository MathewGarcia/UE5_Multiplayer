// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "MainMenuGameModeBase.h"


ALobbyPlayerController::ALobbyPlayerController()
{
    bReplicates = true;
}

void ALobbyPlayerController::ServerAssignTeamToPlayer_Implementation()
{
    AssignTeamToPlayer();
}

bool ALobbyPlayerController::ServerAssignTeamToPlayer_Validate()
{
    return true;
}

void ALobbyPlayerController::AssignTeamToPlayer()
{
        AMainMenuGameModeBase* GM = Cast<AMainMenuGameModeBase>(GetWorld()->GetAuthGameMode());
        if (GM)
        {
            GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Red, FString::Printf(TEXT("[Server] Assigning team to player.")));
            GM->AssignTeamToPlayer(this);
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Red, FString::Printf(TEXT("[Server] GM FAILED")));
        }


}


void ALobbyPlayerController::BeginPlay()
{
    Super::BeginPlay();
    GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Green, FString::Printf(TEXT("Calling Begin play on lobby player controller")));
    if (HasAuthority()) {
        AssignTeamToPlayer();
    }

}
