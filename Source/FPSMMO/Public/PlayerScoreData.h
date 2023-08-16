// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerScoreData.generated.h"
/**
 * 
 */
UCLASS(Blueprintable)
class FPSMMO_API UPlayerScoreData : public UObject
{

	GENERATED_BODY()
public:
	UPlayerScoreData();
	~UPlayerScoreData();
	UPROPERTY(BlueprintReadWrite, Category = "Player")
		FString PlayerName;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
		int32 Kills;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
		int32 Deaths;

};
