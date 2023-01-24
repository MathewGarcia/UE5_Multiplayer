// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"
#include "FPSCharacter.h"
#include "pHUD.h"

AFPSPlayerController::AFPSPlayerController()
{


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
		HUDWidget->text = text;
		HUDWidget->UpdateText();
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
	}
}
