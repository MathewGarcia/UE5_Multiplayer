// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

class AWeapon;
class ACaptureRing;
class APlayerInfoState;
class AFPSMMOGameModeBase;
struct FDeadPlayerInfo;
class AFPSCharacter;
class ABombSite;
class ABomb;
class ALane;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayersChanged);

UCLASS()
class FPSMMO_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AFPSGameState();

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


		UPROPERTY(BlueprintReadOnly, ReplicatedUsing = "OnRep_UpdatePlayerArray")
			TArray<APlayerInfoState*>PlayerStates;

		UFUNCTION()
			void OnRep_UpdatePlayerArray();


		UPROPERTY(EditAnywhere, Category = "All Weapons")
			TArray<TSubclassOf<AWeapon>> Weapons;

		UPROPERTY(EditAnywhere, Category = "Ring To Spawn")
			TSubclassOf<ACaptureRing> CaptureRingToSpawn;

		UPROPERTY(Replicated)
			ACaptureRing* SpawnedCaptureRing;

		UFUNCTION(Server, Reliable, WithValidation)
			void ServerDestroyRing();

	void DestroyRing() const;

		void SpawnCaptureRing();

		FTimerHandle DestroyTimerHandle;

		FTimerHandle SpawnRingTimerHandle;

		UPROPERTY(EditAnywhere, Category = "Timer")
			float SpawnRingTimer;

		UPROPERTY(Replicated,EditAnywhere, Category = "TopLane")
			ALane* TopLane;

		UPROPERTY(Replicated,EditAnywhere, Category = "MidLane")
			ALane* MidLane;

		UPROPERTY(Replicated,EditAnywhere, Category = "BotLane")
			ALane*BotLane;

		bool CanCapture(ACaptureRing*currentRing);

		void InitLanes();

		UFUNCTION(Server, Reliable)
		void ServerInitLanes();

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerRingUpdate(ACaptureRing* ring);
		void ServerRingUpdate_Implementation(ACaptureRing* ring);
		bool ServerRingUpdate_Validate(ACaptureRing* ring);

		void RingUpdate(ACaptureRing*ring);
private:
	UPROPERTY(Replicated)
	ABomb* Bomb;

	UPROPERTY()
	AFPSMMOGameModeBase* GM;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags);

	virtual void BeginPlay() override;
};
