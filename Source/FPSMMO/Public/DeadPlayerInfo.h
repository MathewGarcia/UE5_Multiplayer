// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeadPlayerInfo.generated.h"
/**
 * 
 */
USTRUCT()
struct FDeadPlayerInfo
{
	GENERATED_BODY()

		AController* PlayerController;
	UPROPERTY()
	float RespawnTimeLeft;

	FDeadPlayerInfo() : PlayerController(nullptr), RespawnTimeLeft(0.0f) {}

	FDeadPlayerInfo(AController* NewController, float RespawnDuration) : PlayerController(NewController), RespawnTimeLeft(RespawnDuration) {}
};
