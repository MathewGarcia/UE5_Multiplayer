// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "pHUD.generated.h"

class APlayerInfoState;
class AFPSCharacter;
/**
 * 
 */
UCLASS()
class FPSMMO_API UpHUD : public UUserWidget
{
	GENERATED_BODY()


		UpHUD(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_UpdateHealth,BlueprintReadWrite, Category = "PlayerInfo")
		float HP;

	UPROPERTY(EditAnywhere, ReplicatedUsing= OnRep_UpdateShield,BlueprintReadWrite,Category = "PlayerInfo")
		float SP;

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	void OnRep_UpdateShield(float Shield);

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
		void OnRep_UpdateHealth(float Health);

	UPROPERTY()
		AFPSCharacter* player;

	UFUNCTION(BlueprintCallable, Category = "PlayerInfoState")
	APlayerInfoState* GetPlayerInfoState();

	UFUNCTION(BlueprintCallable, Category = "UpdateText")
		FText UpdateText();

	UFUNCTION()
		void SetText(FText newText);

private:
	APlayerState* PS;
	FText text;


protected:
	virtual void NativeConstruct() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
