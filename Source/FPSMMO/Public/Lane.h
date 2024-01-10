// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Lane.generated.h"

/**
 * 
 */
class ACaptureRing;
enum class ELane :uint8;

UCLASS()
class FPSMMO_API ULane : public UObject
{
	GENERATED_BODY()


public:
	ULane* LChild;
	ULane* RChild;

	ACaptureRing* ring;

	void Delete(ACaptureRing*ringToDelete);
	void Insert(ACaptureRing*ringToAdd);

	bool CanCapture(ACaptureRing* currentRing);
};
