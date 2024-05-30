// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerInfoState.generated.h"

class AFPSGameState;
enum class ETeam : uint8;
class AFPSCharacter;
/**
 * 
 */
UENUM(Blueprintable)
enum class EConnection : uint8 {
	Lobby UMETA(DisplayName = "Lobby"),
	Connecting UMETA(DisplayName = "Connecting"),
	Connected UMETA(DisplayName = "Connected")
};
UCLASS()
class FPSMMO_API APlayerInfoState : public APlayerState
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Team")
		ETeam TeamId;

	APlayerInfoState();

	FName PlayerName = "MAT";

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

	UFUNCTION(BlueprintCallable, Category="Gold")
	int GetGold();

	//server SetGold
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetGold(int32 NewGold);

	void SetGold(int gold);

	void UpdateGold(int gold);


	void SetPlayerController(APlayerController* Controller);


	AFPSCharacter*GetPlayer();

	void SetPlayer(AFPSCharacter* Player);

	void UpdateKills();
	void UpdateDeath();

	int32 GetKills();

	int32 GetDeaths();

	UFUNCTION()
		void OnRep_UpdateKills();

	UFUNCTION()
		void OnRep_UpdateDeaths();

	int32 GetSlotLocation();

	void SetSlotLocation(int32 NewSlotLocation);

	UPROPERTY()
	bool bIsInLobby;

	EConnection GetConnectionState();

	void SetConnectionState(EConnection newConnectionState);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUpgradeShield();

	UPROPERTY(Replicated)
	float MaxShield = 100;
private:
	UPROPERTY(Replicated)
		int Gold;

	APlayerController* PC;

	EConnection ConnectionState;

	AFPSCharacter* player;

	UPROPERTY(ReplicatedUsing= "OnRep_UpdateKills")
	int32 Kills = 0;

	UPROPERTY(ReplicatedUsing = "OnRep_UpdateDeaths")
	int32 Deaths = 0;

	UPROPERTY(Replicated)
	int32 ConsecutiveKills = 0;

	AFPSGameState* GS;

	UPROPERTY(Replicated)
		int32 SlotLocation = 0;
protected:

	UPROPERTY(EditAnywhere,ReplicatedUsing = OnRep_Level, Category = "PlayerInfo")
		int CurrentLevel;


	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_InCombat, Category = "PlayerInfo")
		bool bInCombat;

	//does not need to be replicated
		float CurrentEXP;

		const int MAX_LEVEL = 5;

		UPROPERTY(BlueprintReadWrite, Category = "PlayerInfoState")
		float EXPNeeded = 100;


	//replicated functions
		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

		virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void CopyProperties(APlayerState* PlayerState) override;


public:
	UFUNCTION()
	void OnRep_Level();

	UFUNCTION()
		void OnRep_InCombat();

	UFUNCTION(Client, Reliable)
		void ClientSetInputModeGameOnly();
};
