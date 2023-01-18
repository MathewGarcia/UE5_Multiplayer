// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class AFPSCharacter;
class UpHUD;
/**
 * 
 */
UCLASS()
class FPSMMO_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
		APlayerHUD();

		UPROPERTY(EditDefaultsOnly ,Category = "pHUDClass")
			TSubclassOf<UUserWidget> pHUDClass;

		virtual void BeginPlay() override;

		virtual void DrawHUD() override;

		UpHUD* HUDWidget = nullptr;
private:

	


	AFPSCharacter* player;

};
