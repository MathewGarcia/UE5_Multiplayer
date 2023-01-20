// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerInfoState.generated.h"

/**
 * 
 */
UCLASS()
class FPSMMO_API APlayerInfoState : public APlayerState
{
	GENERATED_BODY()

public:
	APlayerInfoState();

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	int GetPlayerLevel();

	void LevelUp();

	void UpdateEXP(float EXP);

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	float GetEXP();

	void SetEXP(float EXP);
protected:

	UPROPERTY(EditAnywhere,ReplicatedUsing = OnRep_Level, Category = "PlayerInfo")
		int CurrentLevel;


	//does not need to be replicated
		float CurrentEXP;

		const int MAX_LEVEL = 5;


		float EXPNeeded = 10;




	//replicated functions
		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

		virtual void BeginPlay() override;
public:
	UFUNCTION()
	void OnRep_Level();
};
