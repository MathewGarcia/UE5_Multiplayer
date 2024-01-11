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
class FPSMMO_API ALane : public AActor
{
	GENERATED_BODY()

public:

	ALane();
	UPROPERTY(Replicated)
	ALane* LChild;

	UPROPERTY(Replicated)
	ALane* RChild;

	ACaptureRing* ring;

	void Delete(ACaptureRing*ringToDelete);
	void Insert(ACaptureRing*ringToAdd);

	bool CanCapture(ACaptureRing* currentRing);

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
