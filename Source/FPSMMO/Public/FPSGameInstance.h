// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FPSGameInstance.generated.h"

class FOnlineSessionSearch;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerJoined);

/**
 * 
 */
UCLASS()
class FPSMMO_API UFPSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	/*void FindGames();

	UFUNCTION()
	void OnFindSessionsComplete(bool bWasSuccessful);

	void OnJoinSessionComplete(FName Name, EOnJoinSessionCompleteResult::Type Result);

	void JoinGame(int32 SessionIndex);

	UFUNCTION()
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void HostGame();*/

	void CheckAndLoad();

	/*FName CurrentSessionName;


	FString FoundSessionName;*/

	FString PlayerName;

	UPROPERTY()
		FOnPlayerJoined OnPlayerJoin;

	/*UFUNCTION()
		void OnConnectionTimeout();*/



	bool bIsInLobby = false;
private:
	TSharedPtr<FOnlineSessionSettings> SessionSettings;

	FTimerHandle ConnectionTimeoutHandle;
	
};
