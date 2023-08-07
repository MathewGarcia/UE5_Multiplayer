// Fill out your copyright notice in the Description page of Project Settings.


#include "MarketItemData.h"



void UMarketItemData::SetQuantity(int32 NewQuantity)
{
	UE_LOG(LogTemp, Warning, TEXT("Setting Quantity: %d"), NewQuantity);

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

//void UMarketItemData::PostInitProperties()
//{
//	Super::PostInitProperties();
//	UE_LOG(LogTemp, Warning, TEXT("PostInit: InitialQuantity: %d"), InitialQuantity);
//
//	CurrentQuantity = InitialQuantity;
//}



