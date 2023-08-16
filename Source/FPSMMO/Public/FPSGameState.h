// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

class APlayerInfoState;
class AFPSMMOGameModeBase;
struct FDeadPlayerInfo;
class AFPSCharacter;
class ABombSite;
class ABomb;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayersChanged);

UCLASS()
class FPSMMO_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
		UPROPERTY(Replicated)
		bool bIsBombPlanted;

		UFUNCTION(Server, Reliable, WithValidation)
			void ServerUpdateDeadPlayers(float DeltaSeconds);

	UFUNCTION()
		void BombTimerEnded(ABomb*bomb);

		void OnBombPlanted(ABomb*bomb);

		FTimerHandle BombTimerHandle;

		float BombTimerDuration = 5.f;

		void SetBomb(ABomb* bomb);

		ABomb* GetBomb();

		UPROPERTY(Replicated)
			TArray<ABombSite*>BombSites;

	//bounty setting
		void SetBounty(AFPSCharacter*BountyPlayer);

		UFUNCTION(NetMulticast, Reliable)
			void MulticastSetBounty(AFPSCharacter* BountyPlayer);

		void SetGameTime(float GameTime);

	UPROPERTY(Replicated)
			float GameTimeInSeconds;

		UPROPERTY(Replicated)
		TArray<FDeadPlayerInfo>DeadPlayers;

		UPROPERTY(BlueprintAssignable, Category = "Events")
			FOnPlayersChanged OnPlayersChanged;

		// Call this method whenever the player array changes
		void PlayersChanged() const;


		UPROPERTY(ReplicatedUsing = "OnRep_UpdatePlayerArray")
			TArray<APlayerInfoState*>PlayerStates;

		UFUNCTION()
			void OnRep_UpdatePlayerArray();

private:
	UPROPERTY(Replicated)
	ABomb* Bomb;

	AFPSMMOGameModeBase* GM;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;
};
