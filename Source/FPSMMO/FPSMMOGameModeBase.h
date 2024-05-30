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

	UFUNCTION()
	void OnPlayerReadyForInitialization(AFPSCharacter* NewCharacter);

	TArray<AFPSPlayerController*> PlayerControllers;

	//respawn times and such
	UPROPERTY(EditAnywhere, Category = "Respawn Times")
	float BeginningTime;
	UPROPERTY(EditAnywhere, Category = "Respawn Times")
	float MiddleTime;
	UPROPERTY(EditAnywhere, Category = "Respawn Times")
	float EndTime;
	UPROPERTY(EditAnywhere, Category= "Respawn Times")
	float OverTime;

	UPROPERTY(EditAnywhere, Category ="Respawn Times")
	float BeginningRT;
	UPROPERTY(EditAnywhere, Category = "Respawn Times")
	float MiddleRT;
	UPROPERTY(EditAnywhere, Category ="Respawn Times")
	float EndRT;
	UPROPERTY(EditAnywhere, Category ="Respawn Times")
	float OverRT;
	UPROPERTY(EditAnywhere, Category ="Friendly Fire")
	bool bFF = false;



private:

	UPROPERTY(EditAnywhere, Category = "Respawning")
		float RespawnTime;

	FTimerHandle RespawnTimer;

	AFPSGameState* GS;

	bool bHasMatchStarted = false;

protected:
		virtual void InitGameState() override;

		virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	void StartMatch();

};
