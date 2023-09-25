


// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerLobbyListItem.h"

#include "PlayerInfoState.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UPlayerLobbyListItem::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	APlayerInfoState* PS = Cast<APlayerInfoState>(ListItemObject);

	if(PS && PlayerNameTextBlock)
	{
		UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ETeam"), true);
		if (!EnumPtr) return;  // handle failure

		FString EnumName = EnumPtr->GetNameByIndex((int32)PS->TeamId).ToString();

		FString text = FString::Printf(TEXT("%s / Slot: %d, Team: %s"), *PS->GetPlayerName(), PS->GetSlotLocation(), *EnumName);

		PlayerNameTextBlock->SetText(FText::FromString(text));
	}
}
