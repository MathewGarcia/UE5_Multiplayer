// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "FPSCharacter.h"
#include "FPSGameState.h"
#include "FPSPlayerController.h"
#include "MarketWidget.h"
#include "pHUD.h"
#include "ScoreboardWidget.h"
#include "TaccomWidget.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

APlayerHUD::APlayerHUD()
{

}

void APlayerHUD::UpdateText(FText text)
{
	if (HUDWidget)
	{
		HUDWidget->SetText(text);
		HUDWidget->UpdateText();
	}
}

void APlayerHUD::OpenTaccom(bool bIsOpened)
{
	if (bIsOpened)
	{
		TaccomWidget->SetVisibility(ESlateVisibility::Visible);
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		TaccomWidget->SetVisibility(ESlateVisibility::Hidden);
		HUDWidget->SetVisibility(ESlateVisibility::Visible);
	}

}

void APlayerHUD::UpdateGameInfoText(FText NewText)
{
	if(HUDWidget)
	{
		HUDWidget->UpdateInformation(NewText);
	}
}

void APlayerHUD::UpdateGameTimeText(FText NewText)
{
	if(HUDWidget)
	{
		HUDWidget->UpdateGameTime();
	}
}

void APlayerHUD::UpdateRespawnTime(float RespawnTimeLeft)
{
	if(HUDWidget)
	{
		HUDWidget->UpdateDeadPlayerTimer(RespawnTimeLeft);
	}
}

void APlayerHUD::ShowScoreboard()
{
	if(ScoreboardWidget && HUDWidget)
	{
		if(player)
		{
			player->bScoreboardOpen = true;
		}
		ScoreboardWidget->SetVisibility(ESlateVisibility::Visible);
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APlayerHUD::HideScoreboard()
{
	if(ScoreboardWidget && HUDWidget)
	{
		if (player)
		{
			player->bScoreboardOpen = false;
		}
		ScoreboardWidget->SetVisibility(ESlateVisibility::Hidden);
		HUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void APlayerHUD::UpdateScoreboard(APlayerInfoState* PlayerInfo)
{
	if(ScoreboardWidget && PlayerInfo)
	{

		ScoreboardWidget->UpdateScoreboardPlayer(PlayerInfo);
	}
}

void APlayerHUD::RefreshScoreboard()
{
	if(ScoreboardWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("RefreshScoreboard called! Refreshing..."));

		ScoreboardWidget->SetupWidget();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Scoreboard Wdiget failed!"));

	}
}

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("APlayerHUD Created Beginplay line 123"));

	FPSPC = Cast<AFPSPlayerController>(GetOwningPlayerController());

	HUDWidget = CreateWidget<UpHUD>(GetWorld(), pHUDClass.LoadSynchronous());
	player = Cast<AFPSCharacter>(GetOwningPawn());

	if (HUDWidget) {
		HUDWidget->AddToViewport();
	}

	TaccomWidget = CreateWidget<UTaccomWidget>(GetWorld(), pTaccomWidgetClass.LoadSynchronous());
	if (TaccomWidget)
	{
		TaccomWidget->AddToViewport();
		TaccomWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	MarketWidget = CreateWidget<UMarketWidget>(GetWorld(), pMarketWidgetClass.LoadSynchronous());
	if (MarketWidget)
	{
		MarketWidget->AddToViewport();
		MarketWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	ScoreboardWidget = CreateWidget<UScoreboardWidget>(GetWorld(), pScoreboardWidgetClass.LoadSynchronous());
	if(ScoreboardWidget)
	{
		ScoreboardWidget->AddToViewport();
		ScoreboardWidget->SetupWidget();
		ScoreboardWidget->SetVisibility(ESlateVisibility::Hidden);
	}


	if (FPSPC)
	{
		FPSPC->OnHUDReady(this);
	}

}
void APlayerHUD::UpdateMarket(UMarketDataAsset* CurrentMarket)
{
	if (MarketWidget)
	{
		MarketWidget->SetMarketDataAsset(CurrentMarket);
		MarketWidget->SetupWidget();
	}
}

void APlayerHUD::ShowMarket()
{
		if (MarketWidget) {
			MarketWidget->SetVisibility(ESlateVisibility::Visible);
			if (FPSPC) {
				bMarketOpen = true;
				FPSPC->SetShowMouseCursor(bMarketOpen);
				FPSPC->SetIgnoreLookInput(bMarketOpen);
				FPSPC->SetIgnoreMoveInput(bMarketOpen);
			}
			if (HUDWidget)
				HUDWidget->SetVisibility(ESlateVisibility::Hidden);
		}
}

void APlayerHUD::HideMarket()
{
	
		if (MarketWidget)
			MarketWidget->SetVisibility(ESlateVisibility::Hidden);
		if (FPSPC) {
			bMarketOpen = false;
			FPSPC->SetShowMouseCursor(bMarketOpen);
			FPSPC->SetIgnoreLookInput(bMarketOpen);
			FPSPC->SetIgnoreMoveInput(bMarketOpen);
		}
		if (HUDWidget)
			HUDWidget->SetVisibility(ESlateVisibility::Visible);
}

bool APlayerHUD::GetMarketOpen()
{
	return bMarketOpen;
}

void APlayerHUD::DrawHUD()
{
	Super::DrawHUD();
}
