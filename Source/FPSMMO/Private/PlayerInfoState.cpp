// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInfoState.h"
#include "FPSCharacter.h"
#include "Net/UnrealNetwork.h"

APlayerInfoState::APlayerInfoState()
{
	PrimaryActorTick.bCanEverTick = true;

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
	if (player) {
		switch (CurrentLevel)
		{
		case 1:player->SetShield(100);
			player->MaxShield = 100;
			return;
		case 2:player->SetShield(200);
			player->MaxShield = 200;
			return;
		case 3: player->SetShield(300);
			player->MaxShield = 300;
			return;
		case 4: player->SetShield(400);
			player->MaxShield = 400;
			return;
		case 5: player->SetShield(500);
			player->MaxShield = 500;
			return;
		default: return;
		}
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
}

void APlayerInfoState::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<AFPSCharacter>(GetPawn());
	UE_LOG(LogTemp, Warning, TEXT("Player Level is : %d"),CurrentLevel);
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
	UE_LOG(LogTemp, Warning, TEXT("Player combat is : %s"), bInCombat ? TEXT("True") : TEXT("False"));

}

void APlayerInfoState::SetPlayerController(APlayerController* Controller)
{
	PC = Controller;
}