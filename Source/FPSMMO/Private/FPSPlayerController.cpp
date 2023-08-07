// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"
#include "FPSCharacter.h"
#include "MarketWidget.h"
#include "pHUD.h"
#include "PlayerInfoState.h"
#include "TaccomWidget.h"

AFPSPlayerController::AFPSPlayerController()
{

	bReplicates = true;
}


void AFPSPlayerController::UpdateHP(float &Health)
{

	if (HUDWidget) {
		if (player)
		{
			
			HUDWidget->OnRep_UpdateHealth(Health);

		}
	}
}

void AFPSPlayerController::UpdateShield(float &Shield)
{
	if (HUDWidget) {
	
		HUDWidget->OnRep_UpdateShield(Shield);
	}
}

void AFPSPlayerController::UpdateText(FText text)
{
	if(HUDWidget)
	{
		HUDWidget->SetText(text);
		HUDWidget->UpdateText();
	}
}

void AFPSPlayerController::OpenTaccom(bool bIsOpened)
{
	if(bIsOpened)
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

void AFPSPlayerController::SetPlayerState(APlayerInfoState* PlayerInfoState)
{
	PIS = PlayerInfoState;
}

void AFPSPlayerController::UpdateMarket(UMarketDataAsset* CurrentMarket)
{
	if (MarketWidget)
	{
		MarketWidget->SetMarketDataAsset(CurrentMarket);
		MarketWidget->SetupWidget();
	}
}

void AFPSPlayerController::ShowMarket()
{
	if (IsLocalPlayerController()) {
		if (MarketWidget) {
			MarketWidget->SetVisibility(ESlateVisibility::Visible);
			bMarketOpen = true;
			SetShowMouseCursor(bMarketOpen);
			SetIgnoreLookInput(bMarketOpen);
			SetIgnoreMoveInput(bMarketOpen);
			if (HUDWidget)
				HUDWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AFPSPlayerController::HideMarket()
{
	if (IsLocalPlayerController()) {
		if (MarketWidget)
			MarketWidget->SetVisibility(ESlateVisibility::Hidden);
		bMarketOpen = false;
		SetShowMouseCursor(bMarketOpen);
		SetIgnoreLookInput(bMarketOpen);
		SetIgnoreMoveInput(bMarketOpen);		
		if (HUDWidget)
			HUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
}


void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController()) {
		HUDWidget = CreateWidget<UpHUD>(this, pHUDClass.LoadSynchronous());
			 player = Cast<AFPSCharacter>(HUDWidget->GetOwningPlayerPawn());

			if (HUDWidget) {
				HUDWidget->AddToViewport();
			}

		TaccomWidget = CreateWidget<UTaccomWidget>(this, pTaccomWidgetClass.LoadSynchronous());
		if(TaccomWidget)
		{
			TaccomWidget->AddToViewport();
			TaccomWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		MarketWidget = CreateWidget<UMarketWidget>(this, pMarketWidgetClass.LoadSynchronous());
		if(MarketWidget)
		{
			MarketWidget->AddToViewport();
			MarketWidget->SetVisibility(ESlateVisibility::Hidden);
		}

	}
}
