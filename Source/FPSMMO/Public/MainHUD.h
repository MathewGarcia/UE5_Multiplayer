// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

class UpHUD;
class AFPSCharacter;
/**
 * 
 */
UCLASS()
class FPSMMO_API AMainHUD : public AHUD
{
	GENERATED_BODY()

		AMainHUD();

	UPROPERTY(EditDefaultsOnly, Category = "pHUDClass")
		TSubclassOf<UpHUD> pHUDClass;

	virtual void BeginPlay() override;

	virtual void DrawHUD() override;

	UpHUD* HUDWidget;

private:

	UUserWidget* UHUDWidget;

	AFPSCharacter* player;
};
