// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MarketItemData.generated.h"
class AWeapon;
/**
 * 
 */

UCLASS(Blueprintable,BlueprintType)
class FPSMMO_API UMarketItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

		UMarketItemData(const FObjectInitializer& ObjectInitializer);

public:
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AWeapon> WeaponClass;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ItemName;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ItemPrice;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
			int32 InitialQuantity;


		void SetQuantity(int32 NewQuantity);

		int32 GetQuantity();

		void InitializeData();

private:

	int32 CurrentQuantity;
protected:
	/*virtual void PostInitProperties() override;*/




};

