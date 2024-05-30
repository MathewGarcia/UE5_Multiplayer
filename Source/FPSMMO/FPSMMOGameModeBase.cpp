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

    UE_LOG(LogTemp, Warning, TEXT("Post Login"));
    if (HasAuthority() && GS)
    {
 
        if (APlayerInfoState* PIS = Cast<APlayerInfoState>(NewPlayer->PlayerState))
        {
            //TODO we need to move this logic to the lobby and some how pass the state information over.
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

    	if(TotalTimeInMin <= BeginningTime)
        {
            RespawnTime = BeginningRT;
        }
        else if(TotalTimeInMin <= MiddleTime)
        {
            RespawnTime = MiddleRT;
        }
        else if(TotalTimeInMin <= EndTime)
        {
            RespawnTime = EndRT;
        }
        else if (TotalTimeInMin <= OverTime)
        {
            RespawnTime = OverRT;
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
                NewCharacter->RecallLocation = SpawnLocation;
               
            }
        }

        return;
    }


	Super::RestartPlayer(NewPlayer);


}

bool AFPSMMOGameModeBase::AllPlayersConnected()
{
    UE_LOG(LogTemp, Warning, TEXT("Calling All Players Connected"));
    for (APlayerState* playerState : GS->PlayerArray) {
        APlayerInfoState* PIS = Cast<APlayerInfoState>(playerState);
        if (!PIS || PIS->GetConnectionState() != EConnection::Connected) {
            return false;
        }
    }
    return true;
}

void AFPSMMOGameModeBase::OnPlayerReadyForInitialization(AFPSCharacter* NewCharacter)
{
    if (NewCharacter && !NewCharacter->GetCurrentWeapon() && NewCharacter->SpawningWeapon) {
        AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(NewCharacter->SpawningWeapon);
        Weapon->SetActorLocation(NewCharacter->GetActorLocation());
        Weapon->SetOwner(NewCharacter);
        NewCharacter->EquipWeapon(Weapon);
        if (APlayerInfoState* PIS = Cast<APlayerInfoState>(NewCharacter->GetPlayerState()))
        {
            NewCharacter->MaxShield = PIS->MaxShield;
        }
        Weapon->SetPickUp(true);
    }
}

void AFPSMMOGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

    //this is updating the time and also setting the respawn time in one function.
    SetRespawnTime();
    if (GS) {
        GS->ServerUpdateDeadPlayers(DeltaSeconds);
    }

    if (!bHasMatchStarted && AllPlayersConnected()) {
        GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Orange, FString::Printf(TEXT("Started Match")));
        StartMatch();
        bHasMatchStarted = true;

    }
}

void AFPSMMOGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

void AFPSMMOGameModeBase::HandleSeamlessTravelPlayer(AController*& C)
{

    Super::HandleSeamlessTravelPlayer(C);

}
//TODO NOT RESPAWNING WITH WEAPON AND SAME ISSUES WITH LISTEN SERVER NOT SPAWNING WITH WEAPON.
void AFPSMMOGameModeBase::StartMatch()
{
    UE_LOG(LogTemp, Warning, TEXT("Starting Match"));
    //get ride of walls, reset the timer in our game etc.
    if (GS) {
        GS->SetGameTime(0);
        GS->InitiateGame();
    } 
    else {
        GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("GS failed")));
    }
}
