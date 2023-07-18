// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TaccomWidget.generated.h"

class UTextBlock;
class APlayerInfoState;
class ACaptureRing;
/**
 * 
 */
UCLASS(Blueprintable)
class FPSMMO_API UTaccomWidget : public UUserWidget
{
	GENERATED_BODY()

public:
		UPROPERTY(BlueprintReadOnly, Category = "EnemyBox",meta = (BindWidget))
		class UVerticalBox* EnemyBox = nullptr;

		UPROPERTY(BlueprintReadOnly,Category = "FriendlyBox", meta = (BindWidget))
		UVerticalBox* FriendlyBox = nullptr;

	UFUNCTION()
		void UpdateWidget();

protected:
	void SetPlayerInfoState();
	virtual void NativeConstruct() override;

private:
	APlayerInfoState* PlayerInfoState = nullptr;

	FTimerHandle PlayerInfoTimer;

	TMap<ACaptureRing*, UTextBlock*>RingTextBlocks;
};
