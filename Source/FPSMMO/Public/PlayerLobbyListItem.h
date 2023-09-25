// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "PlayerLobbyListItem.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS(Blueprintable)
class FPSMMO_API UPlayerLobbyListItem : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

		UPROPERTY(meta = (BindWidget))
		UTextBlock* PlayerNameTextBlock;

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
};
