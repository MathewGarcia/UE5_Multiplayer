// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

class UMainMenuWidget;
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

	UPROPERTY(EditDefaultsOnly, Category = "MainMenuWidgetClass")
		TSoftClassPtr<UMainMenuWidget> MainMenuWidgetClass;
	UPROPERTY()
	UMainMenuWidget* MainMenuWidget;

	virtual void BeginPlay() override;

	virtual void DrawHUD() override;


public:
	UFUNCTION()
	void RefreshPlayerList();

	void ResetUISettings();
private:


	AFPSCharacter* player;
};
