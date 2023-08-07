// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

class ABombSite;
class ABomb;
/**
 * 
 */
UCLASS()
class FPSMMO_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
		UPROPERTY(Replicated)
		bool bIsBombPlanted;

	UFUNCTION()
		void BombTimerEnded(ABomb*bomb);

		void OnBombPlanted(ABomb*bomb);

		FTimerHandle BombTimerHandle;

		float BombTimerDuration = 5.f;

		void SetBomb(ABomb* bomb);

		ABomb* GetBomb();

		UPROPERTY(Replicated)
			TArray<ABombSite*>BombSites;
private:
	UPROPERTY(Replicated)
	ABomb* Bomb;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;
};
