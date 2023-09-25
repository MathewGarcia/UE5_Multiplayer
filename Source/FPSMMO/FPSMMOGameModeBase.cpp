// Copyright Epic Games, Inc. All Rights Reserved.


#include "FPSMMOGameModeBase.h"

#include "FPSCharacter.h"
#include "FPSGameState.h"
#include "PlayerInfoState.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "TeamEnum.h"
#include "DeadPlayerInfo.h"
#include "FPSPlayerController.h"
#include "PlayerHUD.h"
#include "Weapon.h"
#include "GameFramework/HUD.h"
#include "Steamworks/Steamv153/sdk/public/steam/steam_api.h"

AFPSMMOGameModeBase::AFPSMMOGameModeBase()
{
    PlayerStateClass = APlayerInfoState::StaticClass();
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.bCanEverTick = true;
}


FVector AFPSMMOGameModeBase::DetermineRespawnLocation(ETeam PlayerTeam)
{
    // Get all the target points in the world.
    TArray<AActor*> FoundTargetPoints;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), FoundTargetPoints);

    TArray<AActor*> RespawnLocations;

    // Filter the target points based on the team.
    FString BaseTag;
    if (PlayerTeam == ETeam::TEAM_RED)
    {
        BaseTag = "RED";
    }
    else if (PlayerTeam == ETeam::TEAM_BLUE)
    {
        BaseTag = "BLUE";
    }

    for (AActor* Actor : FoundTargetPoints)
    {
        for (int32 i = 1; i <= 4; ++i)  // Assuming you have up to 4 spawn points for now.
        {
            FString Tag = BaseTag + FString::FromInt(i);
            if (Actor->ActorHasTag(FName(*Tag)))
            {
                RespawnLocations.Add(Actor);
            }
        }
    }

    // Pick a random respawn location from the filtered list.
    if (RespawnLocations.Num() > 0)
    {
        int32 Index = FMath::RandRange(0, RespawnLocations.Num() - 1);
        return RespawnLocations[Index]->GetActorLocation();
    }
        // Default spawn location if none found.
        return FVector::ZeroVector;
}


APawn* AFPSMMOGameModeBase::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
    APlayerInfoState* PlayerInfoState = NewPlayer->GetPlayerState<APlayerInfoState>();
    if (PlayerInfoState)
    {
        FVector SpawnLocation = DetermineRespawnLocation(PlayerInfoState->TeamId);
        FActorSpawnParameters SpawnParameters;
        SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        return GetWorld()->SpawnActor<AFPSCharacter>(BP_PlayerClass, SpawnLocation, FRotator(0.0f), SpawnParameters);
    }

    // Fallback to the default spawning method if something goes wrong.
    return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);

}

void AFPSMMOGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (HasAuthority() && GS)
    {
        if (APlayerInfoState* PIS = Cast<APlayerInfoState>(NewPlayer->PlayerState))
        {
            UE_LOG(LogTemp, Warning, TEXT("PIS Value: %s"), *PIS->GetName());
            GS->PlayerStates.Add(PIS);
        }
    }
}

float AFPSMMOGameModeBase::GetRespawnTime()
{
    return RespawnTime;
}

void AFPSMMOGameModeBase::SetRespawnTime()
{
    UWorld* World = GetWorld();
    if(World)
    {
    
            if (GS)
            {
                float CurrentTime = World->GetTimeSeconds();
                GS->SetGameTime(CurrentTime);
            }
        
        float TotalTimeInSeconds = World->GetTimeSeconds();
        float TotalTimeInMin = TotalTimeInSeconds / 60.f;

    	if(TotalTimeInMin <= 5.0f)
        {
            RespawnTime = 5.0f;
        }
        else if(TotalTimeInMin <= 10.f)
        {
            RespawnTime = 8.f;
        }
        else if(TotalTimeInMin <= 15.f)
        {
            RespawnTime = 12.0f;
        }
        else if (TotalTimeInMin <= 20.0f)
        {
            RespawnTime = 18.f;
        }
    }
}


void AFPSMMOGameModeBase::InitGameState()
{
	Super::InitGameState();

    GS = GetGameState<AFPSGameState>();
    if(GS)
    {
        GS->bIsBombPlanted = false;
    }
}


void AFPSMMOGameModeBase::RestartPlayer(AController* NewPlayer)
{

    if (NewPlayer && HasAuthority())
    {
        APlayerInfoState* PlayerInfoState = NewPlayer->GetPlayerState<APlayerInfoState>();
        if (PlayerInfoState) {
            FVector SpawnLocation = DetermineRespawnLocation(PlayerInfoState->TeamId);
            FActorSpawnParameters SpawnParameters;
            SpawnParameters.Owner = NewPlayer;
            SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            AFPSCharacter* NewCharacter = GetWorld()->SpawnActor<AFPSCharacter>(BP_PlayerClass, SpawnLocation, FRotator(0.0f), SpawnParameters);
            if (NewCharacter)
            {
                NewPlayer->Possess(NewCharacter);
                NewCharacter->OnPlayerReady.AddDynamic(this, &AFPSMMOGameModeBase::OnPlayerReadyForInitialization);
                NewCharacter->PrepareForInitialization();
            }
        }

        return;
    }


	Super::RestartPlayer(NewPlayer);


}

bool AFPSMMOGameModeBase::AllPlayersConnected()
{
	if(GS)
    {
        return GS->PlayerArray.Num() == 2;
    }
    return false;
}

void AFPSMMOGameModeBase::OnPlayerReadyForInitialization(AFPSCharacter* NewCharacter)
{
    if (NewCharacter && !NewCharacter->GetCurrentWeapon() && NewCharacter->SpawningWeapon) {
        AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(NewCharacter->SpawningWeapon);
        Weapon->SetActorLocation(NewCharacter->GetActorLocation());
        Weapon->SetOwner(NewCharacter);
        NewCharacter->EquipWeapon(Weapon);
        Weapon->SetPickUp(true);
    }
}

void AFPSMMOGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

    SetRespawnTime();
    if (GS) {
        GS->ServerUpdateDeadPlayers(DeltaSeconds);
    }

}

void AFPSMMOGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}
