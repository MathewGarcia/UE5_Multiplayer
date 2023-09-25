// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"

// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameInstance.h"
#include "MainMenuGameStateBase.h"
#include "MainMenuWidget.h"
#include "PlayerHUD.h"
#include "PlayerInfoState.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

AMainHUD::AMainHUD()
{

}

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	MainMenuWidget = CreateWidget<UMainMenuWidget>(GetOwningPlayerController(), MainMenuWidgetClass.LoadSynchronous());

	if(MainMenuWidget)
	{
		MainMenuWidget->AddToViewport();
		MainMenuWidget->SetupWidget();
	}

	UFPSGameInstance* GI = Cast<UFPSGameInstance>(GetGameInstance());

	if(GI)
	{
		GI->OnPlayerJoin.AddDynamic(this, &AMainHUD::RefreshPlayerList);
	}
	GetOwningPlayerController()->SetInputMode(FInputModeGameAndUI());
}

void AMainHUD::DrawHUD()
{
	Super::DrawHUD();


}

void AMainHUD::RefreshPlayerList()
{
	if(MainMenuWidget)
	{
		GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("Refresh player list called")));

		MainMenuWidget->SetupWidget();
	}
}
