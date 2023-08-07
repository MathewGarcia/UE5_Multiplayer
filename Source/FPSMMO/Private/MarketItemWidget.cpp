// Fill out your copyright notice in the Description page of Project Settings.


#include "MarketItemWidget.h"

#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "MarketItemData.h"
#include "PlayerInfoState.h"
#include "Weapon.h"
#include "Components/TextBlock.h"


void UMarketItemWidget::OnClickButton()
{
	AFPSPlayerController* FPSPC = Cast<AFPSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if(FPSPC)
	{
		AFPSCharacter* player = Cast<AFPSCharacter>(FPSPC->GetCharacter());
		if(player)
		{
			if (ItemData)
			{
				APlayerInfoState* PIS = Cast<APlayerInfoState>(player->GetPlayerState());
				if (PIS)
				{
					//if we have enough gold we can buy the item
					if (PIS->GetGold() >= ItemData->ItemPrice && ItemData->GetQuantity() > 0) {
						int32 NewGold = PIS->GetGold() - ItemData->ItemPrice;
						PIS->ServerSetGold(NewGold);
						int32 NewQuantity = ItemData->GetQuantity() - 1;
						ItemData->SetQuantity(NewQuantity);
						player->ServerSpawnWeapon(ItemData->WeaponClass);
						UpdateText();
					}
				}
			}
		}
	}
}

void UMarketItemWidget::UpdateText()
{
	if(ItemData)
	{
		QuantityTextBlock->SetText(FText::FromString(FString::FromInt(ItemData->GetQuantity())));
	}
}

void UMarketItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	ItemData = Cast<UMarketItemData>(ListItemObject);
	if(ItemData)
	{
		ItemData->InitializeData();
		ItemNameTextBlock->SetText(FText::FromString(ItemData->ItemName));
		CostTextBlock->SetText(FText::FromString(FString::FromInt(ItemData->ItemPrice)));
		QuantityTextBlock->SetText(FText::FromString(FString::FromInt(ItemData->GetQuantity())));

	}
}

void UMarketItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button->OnClicked.AddDynamic(this, &UMarketItemWidget::OnClickButton);
}
