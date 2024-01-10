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
#include "Lane.h"


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
		//timer for how long the tower will stay alive for.
		GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AFPSGameState::ServerDestroyRing, 120.f);
}



bool AFPSGameState::CanCapture(ACaptureRing* currentRing)
{
	if (currentRing == SpawnedCaptureRing) return true;

	bool result = false;
	switch (currentRing->lane) {

	case ELane::ELane_Top:
		result = TopLane->CanCapture(currentRing);
		break;

	case ELane::ELane_Mid:
		result = MidLane->CanCapture(currentRing);
		break;
	case ELane::ELane_Bot:
		result = BotLane->CanCapture(currentRing);
		break;

	default:
		//this will be for spawned in rings.
		return true;
		
	}

	return result;
}


void AFPSGameState::InitLanes()
{
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACaptureRing::StaticClass(), Actors);

	//lambda function to sort the actors based off their position
	Actors.Sort([](const AActor& A, const AActor& B) {
		//pointer to ring references
		const ACaptureRing* RingA = Cast<ACaptureRing>(&A);
		const ACaptureRing* RingB = Cast<ACaptureRing>(&B);
		//if ringA and ringB exist
		if (RingA && RingB) {
			//return the greater position
			return RingA->pos > RingB->pos;
		}
		return false;
		});

	for (AActor* ringActor : Actors) {
		ACaptureRing* ring = Cast<ACaptureRing>(ringActor);

		switch (ring->lane) {

		case ELane::ELane_Top:
			TopLane->Insert(ring);
			break;

		case ELane::ELane_Mid:
			MidLane->Insert(ring);
			break;
		case ELane::ELane_Bot:
			BotLane->Insert(ring);
			break;

		default:
			//this will be for spawned in rings.
			break;
		}
	}


}

void AFPSGameState::RingUpdate(ACaptureRing* ring)
{
	switch (ring->lane) {

	case ELane::ELane_Top:
		TopLane->Delete(ring);
		break;

	case ELane::ELane_Mid:
		MidLane->Delete(ring);
		break;
	case ELane::ELane_Bot:
		BotLane->Delete(ring);
		break;

	default:
		//this will be for spawned in rings.
		break;
	}
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
	DOREPLIFETIME(AFPSGameState, TopLane);
	DOREPLIFETIME(AFPSGameState, MidLane);
	DOREPLIFETIME(AFPSGameState, BotLane);
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
	TopLane = NewObject<ULane>(this);
	MidLane = NewObject<ULane>(this);
	BotLane = NewObject<ULane>(this);
	InitLanes();

}
