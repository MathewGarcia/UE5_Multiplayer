// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TeamEnum.h"
#include "GameFramework/GameModeBase.h"
#include "FPSMMOGameModeBase.generated.h"

/**
 * 
 */
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

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

		virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
		virtual void InitGameState() override;
};
