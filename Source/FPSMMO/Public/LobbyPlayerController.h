// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FPSMMO_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

		ALobbyPlayerController();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerAssignTeamToPlayer();

	void AssignTeamToPlayer();

protected:
	virtual void BeginPlay() override;

	
};
