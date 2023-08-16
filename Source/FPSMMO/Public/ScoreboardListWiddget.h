// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "ScoreboardListWiddget.generated.h"

class UPlayerScoreData;
class UTextBlock;
/**
 * 
 */
UCLASS(Blueprintable)
class FPSMMO_API UScoreboardListWiddget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
		UPROPERTY(BlueprintReadWrite, Category = "PlayerInfoTextBlock" , meta = (BindWidget))
		UTextBlock* PlayerInfoTextBlock;

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
};
