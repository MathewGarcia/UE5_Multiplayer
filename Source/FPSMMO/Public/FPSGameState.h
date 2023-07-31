// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

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

		void BombTimerEnded();

		void OnBombPlanted();

		FTimerHandle BombTimerHandle;

		float BombTimerDuration = 5.f;

		void SetBomb(ABomb* bomb);

		ABomb* GetBomb();
private:
	UPROPERTY(Replicated)
	ABomb* Bomb;
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;
};
