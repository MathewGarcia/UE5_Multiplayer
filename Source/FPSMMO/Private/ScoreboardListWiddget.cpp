// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreboardListWiddget.h"

#include "PlayerInfoState.h"
#include "Components/TextBlock.h"


void UScoreboardListWiddget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);


	APlayerInfoState*PS = Cast<APlayerInfoState>(ListItemObject);

	if (PS)
	{
		if (PlayerInfoTextBlock)
		{
		
			FString playerScoreString = FString::Printf(TEXT("%s / %d / %d"), *PS->GetPlayerName(), PS->GetKills(), PS->GetDeaths());
			PlayerInfoTextBlock->SetText(FText::FromString(playerScoreString));
		}
	}
}

