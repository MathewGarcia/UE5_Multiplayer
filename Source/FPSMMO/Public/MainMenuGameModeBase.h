// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSGameState.h"
#include "TeamEnum.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameModeBase.generated.h"

class AMainMenuGameStateBase;
class APlayerInfoState;
/**
 * 
 */
UCLASS()
class FPSMMO_API AMainMenuGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


	int32 RedTeamPlayerCount = 0;
	int32 BlueTeamPlayerCount = 0;
	AMainMenuGameStateBase* GS;

public:

	AMainMenuGameModeBase();

		ETeam AssignTeam(APlayerInfoState*PlayerInfoState);

		void AssignTeamToPlayer(APlayerController* NewPlayer);

		int32 CalculateNextSlot(int32 lastSlot);

protected:
	virtual void BeginPlay() override;

	virtual void OnPostLogin(AController* NewPlayer) override;
};
