// Fill out your copyright notice in the Description page of Project Settings.


#include "pHUD.h"

#include "DeadPlayerInfo.h"
#include "FPSCharacter.h"
#include "FPSGameState.h"
#include "PlayerInfoState.h"


UpHUD::UpHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UpHUD::WidgetUpdateShield(float val)
{
	SP = val;
	UE_LOG(LogTemp, Warning, TEXT("Shield: %f"), val);

}

void UpHUD::WidgetUpdateHealth(float val)
{
	HP = val;
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), val);
}

void UpHUD::SetCharacter(AFPSCharacter* NewCharacter)
{
	player = NewCharacter;
	WidgetUpdateShield(NewCharacter->GetShield());
	WidgetUpdateHealth(NewCharacter->GetHealth());
}

APlayerInfoState* UpHUD::GetPlayerInfoState()
{
	if(player)
	{
		return Cast<APlayerInfoState>(player->GetPlayerState());
	}
	return nullptr;
}

FText UpHUD::UpdateText()
{
	return text;
}

void UpHUD::SetText(FText newText)
{
	text = newText;
}

void UpHUD::NativeConstruct()
{
	Super::NativeConstruct();

	player = Cast<AFPSCharacter>(GetOwningPlayerPawn());

	if(player)
	{
		WidgetUpdateShield(player->GetShield());
		WidgetUpdateHealth(player->GetHealth());
	}

	GetWorld()->GetTimerManager().SetTimer(GameTimeUpdateTimer, this, &UpHUD::UpdateGameTime, 1.0f, true);


}
void UpHUD::ResetHUD()
{
	WidgetUpdateHealth(player->MaxHP);
	WidgetUpdateShield(player->MaxShield);
}

void UpHUD::UpdateInformation(FText NewText)
{	
	if(InformationTextBlock)
	{
		InformationTextBlock->SetText(NewText);
	}
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,this,&UpHUD::ResetInformation,2.0f, false);
}

void UpHUD::ResetInformation()
{
	if(InformationTextBlock)
	{
		InformationTextBlock->SetText(FText::FromString(""));
	}
}

void UpHUD::UpdateGameTime()
{
	AFPSGameState* FPSGameState = Cast<AFPSGameState>(GetWorld()->GetGameState());
	if (FPSGameState)
	{
		int32 TotalSeconds = static_cast<int32>(FPSGameState->GameTimeInSeconds);
		int32 Minutes = TotalSeconds / 60;
		int32 Seconds = TotalSeconds % 60;
		FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		if (GameTimeTextBlock)
		{
			GameTimeTextBlock->SetText(FText::FromString(TimeString));
		}
	}
}

void UpHUD::UpdateDeadPlayerTimer(float RespawnTimeLeft)
{
	
		AFPSGameState* FPSGameState = Cast<AFPSGameState>(GetWorld()->GetGameState());

		if (FPSGameState)
		{
			for (const FDeadPlayerInfo& DeadPlayer : FPSGameState->DeadPlayers)
			{
				if(RespawnTimerTextBlock)
				{
					FString RespawnTimeLeftText = FString::Printf(TEXT("%d"), static_cast<int32>(RespawnTimeLeft));
					if (RespawnTimeLeft > 0) {
						RespawnTimerTextBlock->SetText(FText::FromString(RespawnTimeLeftText));
					} else
					{
						RespawnTimerTextBlock->SetText(FText::FromString(""));

					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("RespawnTimerTextBlock failed"));

				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameState failed in UpHUD"));
		}

}

void UpHUD::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(GameTimeUpdateTimer);

	Super::NativeDestruct();
}

