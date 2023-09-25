// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

#include "FPSGameInstance.h"
#include "LobbyPlayerController.h"
#include "MainHUD.h"
#include "MainMenuGameStateBase.h"
#include "PlayerInfoState.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/Overlay.h"

class AGameSession;

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}


void UMainMenuWidget::FindGame()
{

	UFPSGameInstance* GI = Cast<UFPSGameInstance>(GetGameInstance());

	if(GI)
	{
		/*
		GI->FindGames();
	*/

		GI->bIsInLobby = true;
		bIsInitialScreen = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GI failed in main menu widget"));
	}


}

void UMainMenuWidget::HostGame()
{

	UFPSGameInstance* GI = Cast<UFPSGameInstance>(GetGameInstance());

	if (GI)
	{
		GI->PlayerName = GetOwningPlayerState()->GetPlayerName();
		/*
		GI->HostGame();
	*/
		GI->bIsInLobby = true;
	}

	SelectionScreenOverlay->SetVisibility(ESlateVisibility::Hidden);

	if (LobbyOverlay)
		LobbyOverlay->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenuWidget::SetupWidget()
{
	if(GS)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player States size %d"), GS->PlayerStates.Num());
		GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("Player States Size: %d"), GS->PlayerStates.Num()));
		if (PlayerListView) {
			for (APlayerInfoState* PS : GS->PlayerStates)
			{
				if (PS && PS->IsValidLowLevel()) {
					GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("Adding PlayerState: %s"), *PS->GetPlayerName()));
					PlayerListView->AddItem(PS);
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("Player state null")));

				}
			}
			PlayerListView->RequestRefresh();

		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GS failed in Main Menu Widget"));
	}

}

void UMainMenuWidget::HandleUI()
{
	if (SelectionScreenOverlay && LobbyOverlay) {
		SelectionScreenOverlay->SetVisibility(ESlateVisibility::Hidden);

		LobbyOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

FReply UMainMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bIsInitialScreen && StartingScreenOverlay && SelectionScreenOverlay)
	{

		//TODO fade out and play cool sound?

		bIsInitialScreen = false;

		StartingScreenOverlay->SetVisibility(ESlateVisibility::Hidden);
		SelectionScreenOverlay->SetVisibility(ESlateVisibility::Visible);
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UMainMenuWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	if (bIsInitialScreen && StartingScreenOverlay && SelectionScreenOverlay)
	{


		bIsInitialScreen = false;

		StartingScreenOverlay->SetVisibility(ESlateVisibility::Hidden);
		SelectionScreenOverlay->SetVisibility(ESlateVisibility::Visible);
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UMainMenuWidget::HandlePlayerJoin(APlayerController* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Handle player join!"));
	
}

void UMainMenuWidget::StartGame()
{


	//call game instance start the game.
	UFPSGameInstance* GI = Cast<UFPSGameInstance>(GetGameInstance());
		if(GI)
		{
			GI->CheckAndLoad();
		}
	
}

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetKeyboardFocus();

	GS = Cast<AMainMenuGameStateBase>(GetWorld()->GetGameState());
	UFPSGameInstance* GI = Cast<UFPSGameInstance>(GetGameInstance());
	
	if (GI)
	{
		if (GI->bIsInLobby)
		{
			LobbyOverlay->SetVisibility(ESlateVisibility::Visible);
			SelectionScreenOverlay->SetVisibility(ESlateVisibility::Hidden);
			StartingScreenOverlay->SetVisibility(ESlateVisibility::Hidden);
			bIsInitialScreen = false;
		}
		else
		{
			LobbyOverlay->SetVisibility(ESlateVisibility::Hidden);
			SelectionScreenOverlay->SetVisibility(ESlateVisibility::Hidden);
			StartingScreenOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}

	if (SelectionScreenOverlay && LobbyOverlay && StartButton)
	{
		SelectionButton->OnClicked.AddDynamic(this, &UMainMenuWidget::FindGame);
		HostSelectionButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HostGame);
		StartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::StartGame);

		if(!GetOwningPlayer()->HasAuthority())
		{
			StartButton->SetIsEnabled(false);
		}
	}
}
