// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "FPSCharacter.h"
#include "pHUD.h"
#include "GameFramework/PlayerController.h"

APlayerHUD::APlayerHUD()
{

}

void APlayerHUD::BeginPlay()
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

void APlayerHUD::DrawHUD()
{
	Super::DrawHUD();

	if (HUDWidget) {
		if (player) {
			HUDWidget->SP = player->GetShield();
			HUDWidget->HP = player->GetHealth();
			UE_LOG(LogTemp, Warning, TEXT("Updating Info"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("player failed"));

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HUD Widget failed - DRAWHUD"));

	}
}
