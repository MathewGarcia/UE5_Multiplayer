// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "pHUD.generated.h"

class UImage;
class UEditableTextBox;
class UOverlay;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerInfo")
		float HP;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "PlayerInfo")
		float SP;

	UPROPERTY(BlueprintReadOnly, Category = "InformationTextBlock", meta = (BindWidget))
		UTextBlock* InformationTextBlock;

	UPROPERTY(BlueprintReadOnly, Category = "GameTimeTextBlock", meta = (BindWidget))
		UTextBlock* GameTimeTextBlock;

	UPROPERTY(BlueprintReadOnly, Category = "RespawnTimerTextBlock", meta = (BindWidget))
		UTextBlock* RespawnTimerTextBlock;

	UPROPERTY(BlueprintReadOnly, Category = "Recall", meta = (BindWidget))
		UProgressBar* RecallProgressBar;

		UPROPERTY(BlueprintReadOnly, Category = "FleshFlash", meta = (BindWidget))
		UImage* FleshFlash;

	UPROPERTY(meta = (BindWidget))
	UOverlay*Menu;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* SensTextBox;

	UFUNCTION()
	void TextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void TextChanged(const FText &Text);

	void SetMenuVisibility(ESlateVisibility VisibilityType);

	void HideFleshFlash();

	UPROPERTY(meta = (BindWidget))
	UImage* ShieldImage;

	UPROPERTY(EditAnywhere,Category = "Shield Mat")
	UMaterialInterface* ShieldMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* ShieldMaterialDynamic;

	TMap<int, FLinearColor> ShieldColors;

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	void WidgetUpdateShield(float val);

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
		void WidgetUpdateHealth(float val);

	void SetCharacter(AFPSCharacter* NewCharacter);

	UPROPERTY()
		AFPSCharacter* player;

	UFUNCTION(BlueprintCallable, Category = "PlayerInfoState")
	APlayerInfoState* GetPlayerInfoState();

	UFUNCTION(BlueprintCallable, Category = "UpdateText")
		FText UpdateText();

	UFUNCTION()
		void SetText(FText newText);

	void ResetHUD();

	//Game Info
	FTimerHandle TimerHandle;
	void UpdateInformation(FText NewText);
	void ResetInformation();

	void UpdateGameTime();
	FTimerHandle GameTimeUpdateTimer;

	void UpdateDeadPlayerTimer(float RespawnTimeLeft);

	void UpdateShieldColor();

	void UpdateRecallPB(float CurrentTime);

	void ShowRecallBar();

	void HideRecallBar();

	void PerformFleshFlash();
private:
	APlayerState* PS;
	FText text;
	int LastPlayerLevel = -1;


protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
};
