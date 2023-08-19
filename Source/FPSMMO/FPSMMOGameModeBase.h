// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TeamEnum.h"
#include "GameFramework/GameModeBase.h"
#include "FPSMMOGameModeBase.generated.h"

class ACaptureRing;
class AWeapon;
class AFPSPlayerController;
class AFPSGameState;
struct FDeadPlayerInfo;
class AFPSCharacter;
/**
 * 
 */

DECLARE_DELEGATE(FPlayerDied);
UCLASS()
class FPSMMO_API AFPSMMOGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPSMMOGameModeBase();
	int32 RedTeamPlayerCount = 0;
	int32 BlueTeamPlayerCount = 0;

	ETeam AssignTeam()
	{
		if(RedTeamPlayerCount < BlueTeamPlayerCount)
		{
			RedTeamPlayerCount++;
			return ETeam::TEAM_RED;
		}
		else if (BlueTeamPlayerCount < RedTeamPlayerCount)
		{
			BlueTeamPlayerCount++;
			return ETeam::TEAM_BLUE;
		}
		else
		{
			RedTeamPlayerCount++;
			return ETeam::TEAM_RED;
		}
	}


	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<AFPSCharacter> BP_PlayerClass;


	FVector DetermineRespawnLocation(ETeam PlayerTeam);

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

		virtual void PostLogin(APlayerController* NewPlayer) override;

		FPlayerDied PlayerDied;

	
		float GetRespawnTime();

		void SetRespawnTime();

		AFPSCharacter* player;

		virtual void RestartPlayer(AController* NewPlayer) override;

		bool AllPlayersConnected();

		TArray<AFPSPlayerController*> PlayerControllers;

		void SpawnRing();

private:

	UPROPERTY(EditAnywhere, Category = "Respawning")
		float RespawnTime;

	FTimerHandle RespawnTimer;

	AFPSGameState* GS;

protected:
		virtual void InitGameState() override;

		virtual void Tick(float DeltaSeconds) override;

};
