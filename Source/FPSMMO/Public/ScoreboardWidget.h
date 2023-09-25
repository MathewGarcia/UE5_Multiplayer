// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoState.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "ScoreboardWidget.generated.h"
class UPlayerScoreData;
/**
 * 
 */
UCLASS()
class FPSMMO_API UScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()
public:
		UPROPERTY(BlueprintReadWrite,meta = (BindWidget))
		UListView* ScoreboardList;

		void SetupWidget();

		void UpdateScoreboardPlayer(APlayerInfoState* PlayerInfoState);
	

};
