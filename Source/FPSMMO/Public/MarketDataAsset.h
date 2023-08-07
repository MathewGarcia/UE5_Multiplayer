// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MarketDataAsset.generated.h"

class UMarketItemData;
/**
 * 
 */
UCLASS(Blueprintable)
class FPSMMO_API UMarketDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UMarketDataAsset();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UMarketItemData*> MarketItems;
	
};
