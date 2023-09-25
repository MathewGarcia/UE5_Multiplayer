

// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameStateBase.h"

#include "FPSGameInstance.h"
#include "MainMenuGameModeBase.h"
#include "PlayerInfoState.h"
#include "Net/UnrealNetwork.h"


void AMainMenuGameStateBase::OnRep_UpdatePlayers()
{
	GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("OnRep_UpdatePlayers called in MainMenugamestateBase.")));

	for(APlayerState*PS : PlayerArray)
	{
		if (APlayerInfoState* PIS = Cast<APlayerInfoState>(PS)) {
			PlayerStates.AddUnique(PIS);
			GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("Player added, %s"), *PIS->GetPlayerName()));
		}
	}

	if(GI)
	{
		if(GI->OnPlayerJoin.IsBound())
		GI->OnPlayerJoin.Broadcast();
	}
}

void AMainMenuGameStateBase::OnRep_lastSlotFilled()
{

}

void AMainMenuGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	GM = Cast<AMainMenuGameModeBase>(GetWorld()->GetAuthGameMode());
	GI = Cast<UFPSGameInstance>(GetGameInstance());
}

void AMainMenuGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainMenuGameStateBase, PlayerStates);
	DOREPLIFETIME(AMainMenuGameStateBase, lastSlotFilled);
}
