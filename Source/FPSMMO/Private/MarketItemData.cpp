// Fill out your copyright notice in the Description page of Project Settings.


#include "MarketItemData.h"



void UMarketItemData::SetQuantity(int32 NewQuantity)
{
	CurrentQuantity = NewQuantity;
}

int32 UMarketItemData::GetQuantity()
{
	return CurrentQuantity;
}

void UMarketItemData::InitializeData()
{
	CurrentQuantity = InitialQuantity;
}



