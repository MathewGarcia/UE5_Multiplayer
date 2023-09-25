// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "Bomb.h"
#include "CaptureRing.h"
#include "DeadPlayerInfo.h"
#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "PlayerInfoState.h"
#include "Engine/TargetPoint.h"
#include "FPSMMO/FPSMMOGameModeBase.h"
#include "Kismet/GameplayStatics.h"
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

void AFPSGameState::SetBounty(AFPSCharacter* BountyPlayer)
{
	if(HasAuthority() && BountyPlayer)
	{
				MulticastSetBounty(BountyPlayer);
				BountyPlayer->SetDeathEXP(BountyPlayer->GetDeathEXP() * 2);
				BountyPlayer->SetDeathGold(BountyPlayer->GetDeathGold() * 2);
	}
}

void AFPSGameState::MulticastSetBounty_Implementation(AFPSCharacter* BountyPlayer)
{
	APlayerInfoState* PS = Cast<APlayerInfoState>(BountyPlayer->GetPlayerState());
	if (PS) {
		FString Newtext = FString::Printf(TEXT("%s IS ON A RAMPAGE"), *PS->PlayerName.ToString());

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			AFPSPlayerController* PC = Cast<AFPSPlayerController>(*It);
			if (PC)
			{
				PC->ClientUpdateBountyUI(FText::FromString(Newtext));
			}

		}
	}
}

void AFPSGameState::SetGameTime(float GameTime)
{
		GameTimeInSeconds = GameTime;
}


void AFPSGameState::PlayersChanged() const
{
		if (OnPlayersChanged.IsBound()) {
			OnPlayersChanged.Broadcast();
			UE_LOG(LogTemp, Warning, TEXT("PlayersChanged called! Broadcasting..."));

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayersChanged is not bound..."));
		}
	
}

void AFPSGameState::OnRep_UpdatePlayerArray()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_UpdatePlayerArray called"));


	// Iterate over the PlayerArray and add to the set
	for (APlayerState* PS : PlayerArray)
	{
		if (APlayerInfoState* PIS = Cast<APlayerInfoState>(PS)) {
			PlayerStates.AddUnique(PIS);
		}
	}

	// Notify that the player list has changed
	PlayersChanged();
}

void AFPSGameState::ServerDestroyRing_Implementation()
{
	DestroyRing();
}

bool AFPSGameState::ServerDestroyRing_Validate()
{
	return true;
}

void AFPSGameState::DestroyRing() const
{
	SpawnedCaptureRing->Destroy();
}

void AFPSGameState::SpawnCaptureRing()
{
		TArray<AActor*>FoundActors;
		TArray<ATargetPoint*>TargetPoints;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), FoundActors);

		if (FoundActors.Num() > 0) {
			for (AActor* Actor : FoundActors)
			{
				if (ATargetPoint* TP = Cast<ATargetPoint>(Actor)) {
					if (TP->ActorHasTag("Ring Spawn"))
					{
						TargetPoints.Add(TP);
					}
				}
			}


			int32 num = FMath::RandRange(0, TargetPoints.Num() - 1);
			int32 RandomWeapon = FMath::RandRange(0, Weapons.Num() - 1);
			SpawnedCaptureRing = GetWorld()->SpawnActor<ACaptureRing>(CaptureRingToSpawn, TargetPoints[num]->GetActorLocation(), TargetPoints[num]->GetActorRotation());
			SpawnedCaptureRing->TeamId = ETeam::TEAM_NONE;
			SpawnedCaptureRing->SetMaxRingPoints(5);
			if (Weapons.Num() > 0) {
				SpawnedCaptureRing->WeaponToDrop = Weapons[RandomWeapon];
			}
		}
		GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AFPSGameState::ServerDestroyRing, 120.f);
}

void AFPSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSGameState, bIsBombPlanted);
	DOREPLIFETIME(AFPSGameState, Bomb);
	DOREPLIFETIME(AFPSGameState, BombSites);
	DOREPLIFETIME(AFPSGameState, GameTimeInSeconds);
	DOREPLIFETIME(AFPSGameState, DeadPlayers);
	DOREPLIFETIME(AFPSGameState, PlayerStates);
	DOREPLIFETIME(AFPSGameState, SpawnedCaptureRing);
}

void AFPSGameState::BeginPlay()
{
	Super::BeginPlay();

	GM = Cast<AFPSMMOGameModeBase>(GetWorld()->GetAuthGameMode());
	FTimerHandle BeginPlayTimerHandle;

	if (HasAuthority()) {
		GetWorld()->GetTimerManager().SetTimer(BeginPlayTimerHandle, this, &AFPSGameState::SpawnCaptureRing, 0.5f, false);
		GetWorld()->GetTimerManager().SetTimer(SpawnRingTimerHandle, this, &AFPSGameState::SpawnCaptureRing, SpawnRingTimer, true);
	}

}
