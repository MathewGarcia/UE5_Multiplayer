// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"

// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "FPSCharacter.h"
#include "MarketWidget.h"
#include "pHUD.h"
#include "TaccomWidget.h"
#include "GameFramework/PlayerController.h"

AMainHUD::AMainHUD()
{

}

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	if (pHUDClass)
	{
		HUDWidget = CreateWidget<UpHUD>(GetWorld(), pHUDClass);
		if (HUDWidget) {
			HUDWidget->AddToViewport();
			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (PC) {
				player = Cast<AFPSCharacter>(PC->GetPawn());
				if (player) {
					HUDWidget->HP = player->GetHealth();
					HUDWidget->SP = player->GetShield();
				}

			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("PC failed"));

			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HUD Widget failed - begin play"));

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("pHUDClass failed"));

	}

}

void AMainHUD::DrawHUD()
{
	Super::DrawHUD();


}
