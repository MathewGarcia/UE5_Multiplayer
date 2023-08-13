// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "Bomb.h"
#include "DeadPlayerInfo.h"
#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "PlayerHUD.h"
#include "PlayerInfoState.h"
#include "FPSMMO/FPSMMOGameModeBase.h"
#include "Net/UnrealNetwork.h"


void AFPSGameState::ServerUpdateDeadPlayers_Implementation(float DeltaSeconds)
{
	if (!HasAuthority()) return;

	if (DeadPlayers.Num() > 0) {
		for (int32 i = 0; i < DeadPlayers.Num(); i++)
		{
			DeadPlayers[i].RespawnTimeLeft -= DeltaSeconds;


			AFPSPlayerController* DeadPlayerPC = Cast<AFPSPlayerController>(DeadPlayers[i].PlayerController);
			if (DeadPlayerPC)
			{
				DeadPlayerPC->ClientUpdateRespawnTime(DeadPlayers[i].RespawnTimeLeft);
			}

			if (DeadPlayers[i].RespawnTimeLeft <= 0.0f)
			{
				if (GM) {
					GM->RestartPlayer(DeadPlayers[i].PlayerController);
					DeadPlayers.RemoveAt(i);
					i--;
				}
			}
		}
	}
}

bool AFPSGameState::ServerUpdateDeadPlayers_Validate(float DeltaSeconds)
{
	return true;
}

void AFPSGameState::BombTimerEnded(ABomb*bomb)
{

		if(bomb)
		{
			bomb->MulticastExplode();
			UE_LOG(LogTemp, Warning, TEXT("Bomb EXPLODED"));
			return;
		}

	
	UE_LOG(LogTemp, Warning, TEXT("Bomb REF FAILED"));

}

void AFPSGameState::OnBombPlanted(ABomb*bomb)
{
	if (bIsBombPlanted) {
		if (bomb) {
			FTimerDelegate TimerDel;
			TimerDel.BindUFunction(this, FName("BombTimerEnded"), bomb);
			GetWorld()->GetTimerManager().SetTimer(BombTimerHandle, TimerDel,  bomb->BombTimer, false);
		}
	}
}

void AFPSGameState::SetBomb(ABomb* bomb)
{
	Bomb = bomb;
}

ABomb* AFPSGameState::GetBomb()
{
	return Bomb;
}

//TODO: fix to show to all players.
void AFPSGameState::SetBounty(AFPSCharacter* BountyPlayer)
{
	if(BountyPlayer)
	{
		if(BountyPlayer->GetPlayerHUD())
		{
			APlayerInfoState* PS = Cast<APlayerInfoState>(BountyPlayer->GetPlayerState());
			if (PS) {
				FString Newtext = FString::Printf(TEXT("%s IS ON A RAMPAGE"), *PS->PlayerName.ToString());
				BountyPlayer->GetPlayerHUD()->UpdateGameInfoText(FText::FromString(Newtext));
				BountyPlayer->SetDeathEXP(BountyPlayer->GetDeathEXP() * 2);
				BountyPlayer->SetDeathGold(BountyPlayer->GetDeathGold() * 2);
			}
		}
	}

}

void AFPSGameState::SetGameTime(float GameTime)
{
		GameTimeInSeconds = GameTime;
}

void AFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSGameState, bIsBombPlanted);
	DOREPLIFETIME(AFPSGameState, Bomb);
	DOREPLIFETIME(AFPSGameState, BombSites);
	DOREPLIFETIME(AFPSGameState, GameTimeInSeconds);
	DOREPLIFETIME(AFPSGameState, DeadPlayers);
}

void AFPSGameState::BeginPlay()
{
	Super::BeginPlay();

	GM = Cast<AFPSMMOGameModeBase>(GetWorld()->GetAuthGameMode());


}
