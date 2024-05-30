// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MarketDataAsset.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class APlayerInfoState;
class UScoreboardWidget;
class AFPSPlayerController;
class UTaccomWidget;
class UMarketWidget;
class AFPSCharacter;
class UpHUD;
/**
 * 
 */
UCLASS()
class FPSMMO_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
		APlayerHUD();


		UPROPERTY(EditDefaultsOnly, Category = "pHUDClass")
			TSoftClassPtr<UpHUD> pHUDClass;

		UPROPERTY(EditDefaultsOnly, Category = "Taccom Widget")
			TSoftClassPtr<UTaccomWidget> pTaccomWidgetClass;

		UPROPERTY(EditDefaultsOnly, Category = "Market Widget")
			TSoftClassPtr<UMarketWidget> pMarketWidgetClass;

		UPROPERTY(EditDefaultsOnly, Category = "Scoreboard")
			TSoftClassPtr<UScoreboardWidget> pScoreboardWidgetClass;

			UPROPERTY()
		UpHUD* HUDWidget;

	UPROPERTY()
		UTaccomWidget* TaccomWidget;

	UPROPERTY()
		UMarketWidget* MarketWidget;

		UPROPERTY()
		UScoreboardWidget* ScoreboardWidget;


		void UpdateMarket(UMarketDataAsset* CurrentMarket);

		void ShowMarket();

		void HideMarket();

		bool GetMarketOpen();

		void UpdateText(FText text);

		void SetMenuVisibility(ESlateVisibility VisibilityType);

	void OpenTaccom(bool bIsOpened);

	void UpdateGameInfoText(FText NewText);

	void UpdateGameTimeText(FText NewText);
	void UpdateRespawnTime(float RespawnTimeLeft);

	void ShowScoreboard();

	void HideScoreboard();

	void UpdateScoreboard(APlayerInfoState*PlayerInfo);

	UFUNCTION()
	void RefreshScoreboard();

private:
	AFPSPlayerController* FPSPC = nullptr;

	bool bMarketOpen;
	
protected:

	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	AFPSCharacter* player;

};
