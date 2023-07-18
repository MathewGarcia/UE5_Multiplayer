// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "Net/UnrealNetwork.h"


void AFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSGameState, bIsBombPlanted);
}

void AFPSGameState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Begin play"));
}
