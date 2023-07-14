// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


/**
 * 
 */


UENUM(BlueprintType)
enum class ETeam : uint8
{
	TEAM_NONE UMETA(DisplayName = "Spectator"),
	TEAM_RED UMETA(DisplayName = "Red Team"),
	TEAM_BLUE UMETA(DisplayName = "Blue Team"),
};

class FPSMMO_API TeamEnum
{
public:
	TeamEnum();
	~TeamEnum();
};
