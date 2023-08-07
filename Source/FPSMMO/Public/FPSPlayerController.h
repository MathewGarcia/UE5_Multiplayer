// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

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


		UPROPERTY(EditDefaultsOnly, Category = "pHUDClass")
			TSoftClassPtr<UpHUD> pHUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "Taccom Widget")
			TSoftClassPtr<UTaccomWidget> pTaccomWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Market Widget")
		TSoftClassPtr<UMarketWidget> pMarketWidgetClass;


		UpHUD* HUDWidget;

		UTaccomWidget* TaccomWidget;

		UMarketWidget* MarketWidget;

		bool bMarketOpen;

		void UpdateHP(float &Health);

		void UpdateShield(float &Shield);

		void UpdateText(FText text);

		AFPSCharacter* player;
		void OpenTaccom(bool bIsOpened);

		void SetPlayerState(APlayerInfoState*PlayerInfoState);
		//Market
		void UpdateMarket(UMarketDataAsset* CurrentMarket);

		void ShowMarket();

		void HideMarket();
protected:

	virtual void BeginPlay() override;


private:
	APlayerInfoState* PIS;


	
};
