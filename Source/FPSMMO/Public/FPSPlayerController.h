// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

class APlayerInfoState;
class UTaccomWidget;
class AFPSCharacter;
class UpHUD;
/**
 * 
 */
UCLASS()
class FPSMMO_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
		AFPSPlayerController();


		UPROPERTY(EditDefaultsOnly, Category = "pHUDClass")
			TSoftClassPtr<UpHUD> pHUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "Taccom Widget")
			TSoftClassPtr<UTaccomWidget> pTaccomWidgetClass;

		UpHUD* HUDWidget;

		UTaccomWidget* TaccomWidget;

		void UpdateHP(float &Health);

		void UpdateShield(float &Shield);

		void UpdateText(FText text);

		AFPSCharacter* player;
		void OpenTaccom(bool bIsOpened);

		void SetPlayerState(APlayerInfoState*PlayerInfoState);

protected:

	virtual void BeginPlay() override;


private:
	APlayerInfoState* PIS;


	
};
