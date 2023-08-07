// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "Bomb.h"
#include "BombSite.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


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

void AFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSGameState, bIsBombPlanted);
	DOREPLIFETIME(AFPSGameState, Bomb);
	DOREPLIFETIME(AFPSGameState, BombSites);
}

void AFPSGameState::BeginPlay()
{
	Super::BeginPlay();

}
