// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "MarketItemWidget.generated.h"

class UMarketItemData;
struct FMarketItemData;
class UTextBlock;
/**
 * 
 */
UCLASS()
class FPSMMO_API UMarketItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
		UPROPERTY(BlueprintReadWrite, Category = "Text Block", meta = (BindWidget))
		UTextBlock* ItemNameTextBlock;

		UPROPERTY(BlueprintReadWrite, Category = "Cost Text Block", meta = (BindWidget))
			UTextBlock* CostTextBlock;

		UPROPERTY(BlueprintReadWrite, Category = "Quantity Text Block", meta = (BindWidget))
			UTextBlock* QuantityTextBlock;

		UPROPERTY(BlueprintReadWrite, Category = "Button", meta = (BindWidget))
			UButton* Button;


		UFUNCTION()
		void OnClickButton();

		void UpdateText();

		virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual void NativeConstruct() override;


private:
	UPROPERTY()
	UMarketItemData* ItemData = nullptr;


};
