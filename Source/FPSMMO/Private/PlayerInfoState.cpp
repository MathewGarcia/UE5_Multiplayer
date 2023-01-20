// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInfoState.h"

#include "Net/UnrealNetwork.h"

APlayerInfoState::APlayerInfoState()
{
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

void APlayerInfoState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerInfoState, CurrentLevel);
}

void APlayerInfoState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Player Level is : %d"),CurrentLevel);
}

void APlayerInfoState::OnRep_Level()
{
	LevelUp();
}
