// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInfoState.h"
#include "FPSCharacter.h"
#include "FPSGameState.h"
#include "PlayerHUD.h"
#include "Net/UnrealNetwork.h"


//TODO:ADD A PROGRESSION SYSTEM WHERE EXPNEEDED INCREASES AN X AMOUNT
//TODO: HAVE A SET AMOUNT OF EXP FOR KILLING SOMEONE AND AMOUNT OF GOLD. FOR WEAPON AS WELL.

APlayerInfoState::APlayerInfoState()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	CurrentLevel = 1;
}

int APlayerInfoState::GetPlayerLevel()
{
	return CurrentLevel;
}

void APlayerInfoState::LevelUp()
{
	if(CurrentLevel != MAX_LEVEL)
	{
		if (CurrentEXP >= EXPNeeded) {
			CurrentLevel++;
			SetEXP(0);
			UpgradeShield();
		}
	}
}

void APlayerInfoState::UpdateEXP(float EXP)
{
	SetEXP(GetEXP() + EXP);
	if(GetEXP() >= EXPNeeded)
	{
		LevelUp();
		EXPNeeded = EXPNeeded * 2.25;
	}
}

float APlayerInfoState::GetEXP()
{
	return CurrentEXP;
}

void APlayerInfoState::SetEXP(float EXP)
{
	CurrentEXP = EXP;
}

void APlayerInfoState::UpgradeShield()
{
	player = Cast<AFPSCharacter>(GetPawn());
	if (player)
	{
		// Increase the MaxShield by 25%
		player->MaxShield = player->MaxShield * 1.25;

		// Set the current shield to the new MaxShield
		player->ServerSetShield(player->MaxShield);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player failed"));
	}
}

bool APlayerInfoState::GetInCombat()
{
	return bInCombat;
}

void APlayerInfoState::SetInCombat(bool CombatCond)
{
	if(GetLocalRole() == ROLE_Authority)
	bInCombat = CombatCond;
	
}

void APlayerInfoState::InCombatFalse()
{
	OnRep_InCombat();
}

void APlayerInfoState::RechargeShield()
{
		player = Cast<AFPSCharacter>(GetPawn());
		if (player) {
			if(player->GetShield() != player->MaxShield)
			player->SetShield(player->GetShield() + 10);
		}
	
	GetWorld()->GetTimerManager().ClearTimer(RechargeHandle);

}

int APlayerInfoState::GetGold()
{
	return Gold;
}

void APlayerInfoState::ServerSetGold_Implementation(int32 NewGold)
{
	Gold = NewGold;
}

bool APlayerInfoState::ServerSetGold_Validate(int32 NewGold)
{
	return true;
}

void APlayerInfoState::SetGold(int gold)
{
	Gold += gold;
}

void APlayerInfoState::UpdateGold(int gold)
{
	SetGold(gold);
}

void APlayerInfoState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerInfoState, CurrentLevel);
	DOREPLIFETIME(APlayerInfoState, bInCombat);
	DOREPLIFETIME(APlayerInfoState, TeamId);
	DOREPLIFETIME(APlayerInfoState, Gold);
	DOREPLIFETIME(APlayerInfoState, Kills);
	DOREPLIFETIME(APlayerInfoState, ConsecutiveKills);
	DOREPLIFETIME(APlayerInfoState, Deaths);


}

void APlayerInfoState::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<AFPSCharacter>(GetPawn());
	GS	= GetWorld()->GetGameState<AFPSGameState>();

}

void APlayerInfoState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

			if (!bInCombat)
			{
				RechargeShield();
			}
	
}

void APlayerInfoState::OnRep_Level()
{
	LevelUp();
}

void APlayerInfoState::OnRep_InCombat()
{
	if(!bInCombat)
	{
		SetInCombat(true);
		GetWorld()->GetTimerManager().SetTimer(RechargeHandle, this, &APlayerInfoState::InCombatFalse, RechargeRate, false);

	}
	else
	{
		SetInCombat(false);

		
	}
}

void APlayerInfoState::SetPlayerController(APlayerController* Controller)
{
	PC = Controller;
}

AFPSCharacter* APlayerInfoState::GetPlayer()
{
	return player;
}

void APlayerInfoState::SetPlayer(AFPSCharacter* Player)
{
	player = Player;
}

void APlayerInfoState::UpdateKills()
{
	if (HasAuthority()) {
		Kills++;
		ConsecutiveKills++;
	}
	UE_LOG(LogTemp, Warning, TEXT("Updated Kills %d"),Kills);

	//if the kills incremented 3 times and the player has not died yet, we want to set off the bounty.
	if(ConsecutiveKills >= 3)
	{
		if(GS && player)
		{
			if (HasAuthority()) {
				GS->SetBounty(player);
			}
		}
	}
}

void APlayerInfoState::UpdateDeath()
{
	Deaths++;
	ConsecutiveKills = 0;
}

int32 APlayerInfoState::GetKills()
{
	return Kills;
}

int32 APlayerInfoState::GetDeaths()
{
	return Deaths;
}

void APlayerInfoState::OnRep_UpdateKills()
{
	if(player)
	{
		if (APlayerHUD* PlayerHUD = player->GetPlayerHUD())
		{
			UE_LOG(LogTemp, Warning, TEXT("OnRep_UpdateKills"));
			PlayerHUD->UpdateScoreboard(this);
		}
	}
}

void APlayerInfoState::OnRep_UpdateDeaths()
{
	if (player)
	{
		if (APlayerHUD* PlayerHUD = player->GetPlayerHUD())
		{
			UE_LOG(LogTemp, Warning, TEXT("OnRep_UpdateDeaths"));
			PlayerHUD->UpdateScoreboard(this);
		}
	}
}
