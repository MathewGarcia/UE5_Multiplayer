// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MainMenuGameStateBase.generated.h"

class UFPSGameInstance;
class AMainMenuGameModeBase;
class APlayerInfoState;
/**
 * 
 */
UCLASS()
class FPSMMO_API AMainMenuGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

		UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_UpdatePlayers)
		TArray<APlayerInfoState*>PlayerStates;

	UFUNCTION()
		void OnRep_UpdatePlayers();

	UPROPERTY()
	AMainMenuGameModeBase* GM;

	UPROPERTY()
		UFPSGameInstance* GI;

	UPROPERTY(ReplicatedUsing = OnRep_lastSlotFilled)
		int32 lastSlotFilled = -1;

	UFUNCTION()
		void OnRep_lastSlotFilled();
protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
