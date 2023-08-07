// Fill out your copyright notice in the Description page of Project Settings.


#include "MarketWidget.h"
#include "MarketDataAsset.h"
#include "MarketItemData.h"
#include "MarketItemWidget.h"



void UMarketWidget::SetupWidget()
{

    if (MarketDataAsset)
    {
        for (UMarketItemData* Item : MarketDataAsset->MarketItems)
        {
            TileView->AddItem(Item);
        }
    }

}

void UMarketWidget::SetMarketDataAsset(UMarketDataAsset* CurrentMarket)
{
	MarketDataAsset = CurrentMarket;
}