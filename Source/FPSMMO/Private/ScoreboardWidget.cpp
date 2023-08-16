// Fill ouice in the Description page of Project Settings.


#include "ScoreboardWidget.h"
#include "PlayerInfoState.h"
#include "FPSGameState.h"
#include "ScoreboardListWiddget.h"
#include "Components/TextBlock.h"


void UScoreboardWidget::SetupWidget()
{
	AFPSGameState* GS = Cast<AFPSGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		UE_LOG(LogTemp, Warning, TEXT("player array size: %d"),GS->PlayerStates.Num());
		for (APlayerInfoState*PIS : GS->PlayerStates)
		{
			if (PIS) {

					if (ScoreboardList)
					{
						ScoreboardList->AddItem(PIS);
						ScoreboardList->RequestRefresh();

					}
				
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("PIS failed in Scoreboard Widget"));
			}
		}
	}
}

void UScoreboardWidget::UpdateScoreboardPlayer(APlayerInfoState* PlayerInfoState)
{
    // Iterate through the items in the list view
    for (int32 i = 0; i < ScoreboardList->GetNumItems(); ++i)
    {
        UObject* ItemObject = ScoreboardList->GetItemAt(i);
        APlayerInfoState* ItemPlayerState = Cast<APlayerInfoState>(ItemObject);

        // If this item corresponds to the player whose info has changed
        if (ItemPlayerState && ItemPlayerState == PlayerInfoState)
        {
            // Find the corresponding widget
            UScoreboardListWiddget* ItemWidget = Cast<UScoreboardListWiddget>(ScoreboardList->GetEntryWidgetFromItem(ItemObject));

            // Update the widget with the new score info
            if (ItemWidget && ItemWidget->PlayerInfoTextBlock)
            {
                FString playerScoreString = FString::Printf(TEXT("%s / %d / %d"), *PlayerInfoState->GetPlayerName(), PlayerInfoState->GetKills(), PlayerInfoState->GetDeaths());
                ItemWidget->PlayerInfoTextBlock->SetText(FText::FromString(playerScoreString));
            }

            break;
        }
    }
}




