// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

class AMainHUD;
class APlayerHUD;
class UMarketDataAsset;
class UMarketWidget;
class APlayerInfoState;
class UTaccomWidget;
class AFPSCharacter;
class UpHUD;
/**
 * 
 */
UCLASS()
class FPSMMO_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
		AFPSPlayerController();

		AFPSCharacter* player;

		void SetPlayerState(APlayerInfoState*PlayerInfoState);

		UFUNCTION(Client, Reliable)
			void ClientUpdateRespawnTime(float RespawnTimeLeft);

		UFUNCTION(Client, Reliable)
			void ClientUpdateBountyUI(const FText& NewText);

		UFUNCTION(Client, Reliable)
			void ClientUpdateScoreboard();

		void OnHUDReady(APlayerHUD* playerHUD);

		void OnMainMenuReady(AMainHUD* MainMenuHUD);
protected:

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	APlayerInfoState* PIS;


	
};
