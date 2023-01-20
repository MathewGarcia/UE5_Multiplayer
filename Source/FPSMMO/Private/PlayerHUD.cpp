// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"
#include "FPSCharacter.h"
#include "pHUD.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

APlayerHUD::APlayerHUD()
{

}

void APlayerHUD::UpdateHealth()
{


}

void APlayerHUD::UpdateShield()
{
	

}

void APlayerHUD::BeginPlay()
{
	Super::BeginPlay();

	//APlayerController* PC = GetOwningPlayerController();
	//player = Cast<AFPSCharacter>(PC->GetPawn());


	//	if (pHUDClass)
	//	{
	//		HUDWidget = CreateWidget<UUserWidget>(PC, pHUDClass.LoadSynchronous());
	//		if (HUDWidget) {
	//			HUDWidget->AddToViewport();
	//			if (PC) {
	//				if (player) {
	//					 UHUDWidget = Cast<UpHUD>(HUDWidget);
	//					UHUDWidget->OnRep_UpdateHealth(player->GetHealth());
	//					UHUDWidget->OnRep_UpdateShield(player->GetShield());
	//				}

	//			}
	//			else
	//			{
	//				UE_LOG(LogTemp, Warning, TEXT("PC failed"));

	//			}
	//		}
	//		else
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("HUD Widget failed - begin play"));

	//		}
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("pHUDClass failed"));

	//	}
	//
}

void APlayerHUD::DrawHUD()
{
	Super::DrawHUD();

	//if (HUDWidget) {
	//	if (player) {
	//		HUDWidget->SP = player->GetShield();
	//		HUDWidget->HP = player->GetHealth();
	//		UE_LOG(LogTemp, Warning, TEXT("Updating Info"));
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("player failed"));

	//	}
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("HUD Widget failed - DRAWHUD"));

	//}
}
