// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "PlayerInfoState.h"
#include "INterfaces/OnlineSessionInterface.h"


#define SEARCH_PRESENCE "SEARCH_PRESENCE"


/*
// Searching for Games
void UFPSGameInstance::FindGames()
{
    UE_LOG(LogTemp, Warning, TEXT("Finding game..."));
    GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("Searching for game...")));

    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(TEXT("Steam"));
    if (OnlineSub)
    {
        IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();

        if (SessionInterface.IsValid())
        {
            // Clear existing delegates
            if (SessionInterface->OnFindSessionsCompleteDelegates.IsBound())
            {
                SessionInterface->OnFindSessionsCompleteDelegates.Clear();
            }

            // Add new delegate
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UFPSGameInstance::OnFindSessionsComplete);

            // Create and set the SessionSearch member variable
            SessionSearch = MakeShareable(new FOnlineSessionSearch());
            SessionSearch->bIsLanQuery = false;
            SessionSearch->MaxSearchResults = 2000000;
            SessionSearch->PingBucketSize = 50;
            SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

            const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();

            SessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
        }
    }
}

void UFPSGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
    UE_LOG(LogTemp, Warning, TEXT("On find sessions complete called"));
    GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("On find sessions complete being called")));

    // Use the class member SessionSearch
    if (SessionSearch.IsValid() && bWasSuccessful && SessionSearch->SearchResults.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game found!"));
        GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("Joined game")));

        FoundSessionName = SessionSearch->SearchResults[0].Session.OwningUserName;

        GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("Found Session Name : %s"), *FoundSessionName));


        // You can join the first found game or present a list to the player
        JoinGame(0);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("No game found")));
    }
}
*/


/*void UFPSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        GetWorld()->GetTimerManager().SetTimer(ConnectionTimeoutHandle, this, &UFPSGameInstance::OnConnectionTimeout, 20.0f, false);

        IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(TEXT("Steam"));
        if (IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface())
        {
            FString ConnectInfo;
            if (SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo))
            {
                APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
                if (PlayerController)
                {
                    bIsInLobby = true;
                    CurrentSessionName = FName(*FoundSessionName);
                    GEngine->AddOnScreenDebugMessage(-1, 200, FColor::White, FString::Printf(TEXT("Joined session with Name : %s"), *CurrentSessionName.ToString()));

                    GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("Joining the game, attempting to send the client.")));
                    GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Green, FString::Printf(TEXT("Traveling to: %s"), *ConnectInfo));

                    PlayerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
                }
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Red, FString::Printf(TEXT("Failed to resolve Connect String")));
            }

        }
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(ConnectionTimeoutHandle);
        UE_LOG(LogTemp, Warning, TEXT("Join session failed."));
    }
}


// Joining a Game
void UFPSGameInstance::JoinGame(int32 Index)
{
    if (!SessionSearch.IsValid()) return;

    if (IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(TEXT("Steam")))
    {
        if (IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface())
        {
            if (SessionSearch->SearchResults.Num() > Index)
            {
                FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
                OnJoinSessionCompleteDelegate.BindUObject(this, &UFPSGameInstance::OnJoinSessionComplete);
                SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

                const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();

                SessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSearch->SearchResults[Index]);
            }
        }
    }
}


void UFPSGameInstance::HostGame()
{
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get(TEXT("Steam"));
    if (OnlineSub)
    {
        IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();

        if (SessionInterface.IsValid())
        {

            // Bind delegate
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UFPSGameInstance::OnCreateSessionComplete);

            FOnlineSessionSettings SessionSettings;
            SessionSettings.bIsLANMatch = false;
            SessionSettings.bUsesPresence = true;
            SessionSettings.bUseLobbiesIfAvailable = true;
            SessionSettings.NumPublicConnections = 32;
            SessionSettings.bShouldAdvertise = true;

            const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();

            // Create session
            if (!SessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSettings))
            {
                // Log an error if the session couldn't be created.
                UE_LOG(LogTemp, Error, TEXT("Could not create session"));
                GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("Could not create session")));

            }
        }
    }
}



void UFPSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        CurrentSessionName = SessionName;
        GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Green, FString::Printf(TEXT("Created session with name : %s"), *CurrentSessionName.ToString()));

        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            bIsInLobby = true;
            GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Blue, FString::Printf(TEXT("Changed level and created session")));
            PlayerController->ClientTravel(TEXT("/Game/Maps/LobbyLevel?listen"), ETravelType::TRAVEL_Absolute);
        }
    }
}*/

void UFPSGameInstance::CheckAndLoad()
{
    //set each player back into game only mode.
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PlayerController = Iterator->Get();
        if (APlayerInfoState* PIS = Cast<APlayerInfoState>(PlayerController->PlayerState))
        {
            PIS->ClientSetInputModeGameOnly(); 
        }
    }   

}

/*
void UFPSGameInstance::OnConnectionTimeout()
{
    // Check your connection status here, for example, bIsInLobby
    if (!bIsInLobby)
    {
        GEngine->AddOnScreenDebugMessage(-1, 200, FColor::Red, FString::Printf(TEXT("Connection Timedout")));
        // Perform whatever action is appropriate for a timeout, such as returning to the main menu
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Successfully connected before timeout."));
    }

    // Clear the timer
    GetWorld()->GetTimerManager().ClearTimer(ConnectionTimeoutHandle);
}
*/

