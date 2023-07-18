// Fill out your copyright notice in the Description page of Project Settings.


#include "TaccomWidget.h"

#include "Kismet/GameplayStatics.h"
#include "CaptureRing.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"


void UTaccomWidget::UpdateWidget()
{
    UE_LOG(LogTemp, Warning, TEXT("Updating Taccom Widget"));
    for(const auto& Pair : RingTextBlocks)
    {
        ACaptureRing* Ring = Pair.Key;
        UTextBlock* RingText = Pair.Value;
        if (Ring && RingText)
        {
            RingText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Ring->GetRingPoints(), Ring->GetMaxRingPoints())));
        }
    }
}

void UTaccomWidget::SetPlayerInfoState()
{
	if (!PlayerInfoState) {
		PlayerInfoState = PlayerInfoState = Cast<APlayerInfoState>(GetOwningPlayerPawn()->GetPlayerState());
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(PlayerInfoTimer);

	TArray<AActor*> FoundActors;
	UE_LOG(LogTemp, Warning, TEXT("Before getting all ACaptureRing actors"));
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACaptureRing::StaticClass(), FoundActors);
	UE_LOG(LogTemp, Warning, TEXT("After getting all ACaptureRing actors. Found %d actors."), FoundActors.Num());

    for (AActor* Actor : FoundActors)
    {
        ACaptureRing* Ring = Cast<ACaptureRing>(Actor);
        if (Ring)
        {

            UTextBlock* RingText = NewObject<UTextBlock>(this);

            Ring->OnRingPointsUpdated.AddDynamic(this, &UTaccomWidget::UpdateWidget);

            if (RingText)
            {
                RingText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Ring->GetRingPoints(), Ring->GetMaxRingPoints())));

                // Add the UTextBlock to the correct VerticalBox and to our map.
                if (Ring->TeamId != PlayerInfoState->TeamId && EnemyBox)
                {
                    EnemyBox->AddChildToVerticalBox(RingText);
                }
                else if (FriendlyBox)
                {
                    FriendlyBox->AddChildToVerticalBox(RingText);
                }

                RingTextBlocks.Add(Ring, RingText);
            }
        }
    }
}

void UTaccomWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetTimerManager().SetTimer(PlayerInfoTimer, this, &UTaccomWidget::SetPlayerInfoState, 0.1f, true);

}


