// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "Bomb.h"
#include "Net/UnrealNetwork.h"


void AFPSGameState::BombTimerEnded()
{
	if(Bomb)
	{
		Bomb->MulticastExplode();
		UE_LOG(LogTemp, Warning, TEXT("Bomb EXPLODED"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Bomb REF FAILED"));

}

void AFPSGameState::OnBombPlanted()
{
	if (bIsBombPlanted) {
		if(Bomb)
		GetWorld()->GetTimerManager().SetTimer(BombTimerHandle, this, &AFPSGameState::BombTimerEnded,Bomb->BombTimer);
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

void AFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSGameState, bIsBombPlanted);
	DOREPLIFETIME(AFPSGameState, Bomb);
}

void AFPSGameState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Begin play"));
}
