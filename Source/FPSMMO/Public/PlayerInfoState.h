// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerInfoState.generated.h"

enum class ETeam : uint8;
class AFPSCharacter;
/**
 * 
 */
UCLASS()
class FPSMMO_API APlayerInfoState : public APlayerState
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Team")
		ETeam TeamId;

	APlayerInfoState();

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	int GetPlayerLevel();

	void LevelUp();

	void UpdateEXP(float EXP);

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	float GetEXP();

	void SetEXP(float EXP);

	void UpgradeShield();

	bool GetInCombat();

	void SetInCombat(bool CombatCond);

	void InCombatFalse();

	FTimerHandle RechargeHandle;

	float RechargeRate = 5;

	void RechargeShield();

	AFPSCharacter* player;

	UFUNCTION(BlueprintCallable, Category="Gold")
	int GetGold();

	//server SetGold
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetGold(int32 NewGold);

	void SetGold(int gold);

	void UpdateGold(int gold);

	int Gold;

	void SetPlayerController(APlayerController* Controller);

	APlayerController* PC;
protected:

	UPROPERTY(EditAnywhere,ReplicatedUsing = OnRep_Level, Category = "PlayerInfo")
		int CurrentLevel;


	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_InCombat, Category = "PlayerInfo")
		bool bInCombat;

	//does not need to be replicated
		float CurrentEXP;

		const int MAX_LEVEL = 5;

		UPROPERTY(BlueprintReadWrite, Category = "PlayerInfoState")
		float EXPNeeded = 10;


	//replicated functions
		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

		virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;


public:
	UFUNCTION()
	void OnRep_Level();

	UFUNCTION()
		void OnRep_InCombat();
};
