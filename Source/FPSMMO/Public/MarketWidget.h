// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TileView.h"
#include "MarketWidget.generated.h"

class UMarketItemWidget;
struct FMarketItemData;
class UMarketDataAsset;
/**
 * 
 */
UCLASS()
class FPSMMO_API UMarketWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTileView*TileView;

	void SetupWidget();

	void SetMarketDataAsset(UMarketDataAsset* CurrentMarket);

private:
	UPROPERTY()
	UMarketDataAsset* MarketDataAsset;



};
