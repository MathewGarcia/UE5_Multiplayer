// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputConfigData.generated.h"

class UInputAction;
/**
 * 
 */
UCLASS(Blueprintable)
class FPSMMO_API UInputConfigData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UInputAction* InputMove;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UInputAction* InputLook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UInputAction* InputFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UInputAction* InputInteraction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UInputAction* InputSwapWeapons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UInputAction* InputSprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UInputAction* InputJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UInputAction* InputCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UInputAction* InputADS;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UInputAction* InputReload;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UInputAction* InputTaccom;


};
