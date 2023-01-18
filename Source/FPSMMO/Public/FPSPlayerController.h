// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

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

		UpHUD* HUDWidget;

		void UpdateHP();

		void UpdateShield(float Shield);

protected:

	virtual void BeginPlay() override;


private:


	
};
