// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "pHUD.generated.h"

/**
 * 
 */
UCLASS()
class FPSMMO_API UpHUD : public UUserWidget
{
	GENERATED_BODY()


		UpHUD(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "PlayerInfo")
		float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "PlayerInfo")
		float SP;

	UFUNCTION(BlueprintCallable, Category = "Player UI")
	void UpdateShield(float Shield);
	UFUNCTION(BlueprintCallable, Category = "Player UI")
		void UpdateHealth(float Health);
};
