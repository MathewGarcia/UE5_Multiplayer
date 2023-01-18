// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"
#include "FPSCharacter.h"
#include "pHUD.h"
#include "GameFramework/PlayerState.h"

AFPSPlayerController::AFPSPlayerController()
{


}


void AFPSPlayerController::UpdateHP()
{
	AFPSCharacter* player = Cast<AFPSCharacter>(GetPawn());

	if (HUDWidget) {
		if (player)
		{
			
			HUDWidget->UpdateHealth(player->GetHealth());

		}
	}
}

void AFPSPlayerController::UpdateShield(float Shield)
{

	if (HUDWidget) {
	/*APlayerState*PS = HUDWidget->GetOwningPlayerPawn()->GetPlayerState();

		if(PS)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player State = %s"), *PS->GetName());
		}*/
			HUDWidget->UpdateShield(Shield);
	}
}

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();


	//UE_LOG(LogTemp, Warning, TEXT("Owner: %s "), *GetOwner()->GetName());
	if (IsLocalPlayerController()) {
		HUDWidget = CreateWidget<UpHUD>(this, pHUDClass.LoadSynchronous());
			AFPSCharacter* player = Cast<AFPSCharacter>(GetPawn());
			if (HUDWidget) {
				HUDWidget->AddToViewport();
				if(player)
				{
					
					HUDWidget->SP = player->GetShield();
					HUDWidget->HP = player->GetHealth();

				}
			}
	}
}
