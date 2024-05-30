// Fill out your copyright notice in the Description page of Project Settings.


#include "pHUD.h"

#include "DeadPlayerInfo.h"
#include "FPSCharacter.h"
#include "FPSGameState.h"
#include "PlayerInfoState.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"

UpHUD::UpHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UpHUD::TextCommitted(const FText&Text, ETextCommit::Type CommitMethod)
{
}

void UpHUD::TextChanged(const FText& Text)
{
	FString InputString = Text.ToString();

	if(InputString.IsNumeric() && FCString::Atof(*InputString) >= 0.0f)
	{
		if(player)
		{
			player->SetSensitivity(FCString::Atof(*InputString));
		}
	}
}

void UpHUD::SetMenuVisibility(ESlateVisibility VisibilityType)
{
	Menu->SetVisibility(VisibilityType);
}

void UpHUD::HideFleshFlash()
{
	if(FleshFlash)
	{
		FleshFlash->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UpHUD::WidgetUpdateShield(float val)
{
	SP = val;
	if (ShieldMaterialDynamic) {
		if (APlayerInfoState*PIS = Cast<APlayerInfoState>(GetOwningPlayerState())) {
			if (LastPlayerLevel != PIS->GetPlayerLevel())
			{

				UpdateShieldColor();
			}
			if (SP >= 1 || SP <= 0)
			{
				ShieldImage->SetOpacity(0.f);
			}
			else if (ShieldImage->GetColorAndOpacity().A <= 0.f)
			{
				ShieldImage->SetOpacity(0.7f);

			}
			ShieldMaterialDynamic->SetScalarParameterValue(FName("ClearRadius"), 1 - SP);
		}
	}

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

		ShieldColors.Add(1, FLinearColor::White);
		ShieldColors.Add(2, FLinearColor::Blue);
		ShieldColors.Add(3, FLinearColor::Green);
		ShieldColors.Add(4, FLinearColor::Red);
		ShieldColors.Add(5, FLinearColor::Yellow);

	}
	if (ShieldImage) {
		ShieldImage->SetOpacity(0.f);
	}
	SensTextBox->OnTextChanged.AddDynamic(this, &UpHUD::TextChanged);
	SensTextBox->OnTextCommitted.AddDynamic(this, &UpHUD::TextCommitted);

	if (ShieldMaterial) {
		ShieldMaterialDynamic = UMaterialInstanceDynamic::Create(ShieldMaterial, this);

		if (ShieldImage && ShieldMaterialDynamic)
		{
			ShieldImage->SetBrushFromMaterial(ShieldMaterialDynamic);

				UpdateShieldColor();
			

			UE_LOG(LogTemp, Warning, TEXT("Created dynamic material"));
		}
	}

	if(RecallProgressBar)
	{
		RecallProgressBar->SetVisibility(ESlateVisibility::Collapsed);
	}
	if(FleshFlash)
	{
		FleshFlash->SetVisibility(ESlateVisibility::Collapsed);
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

void UpHUD::UpdateShieldColor()
{
	AFPSCharacter* Player = Cast<AFPSCharacter>(GetOwningPlayerPawn());
	if (Player && Player->IsLocallyControlled()) {
		if (APlayerInfoState* PIS = Cast<APlayerInfoState>(GetOwningPlayerState())) {
			int PlayerLevel = PIS->GetPlayerLevel();
			if (ShieldColors.Contains(PlayerLevel))
			{
				ShieldMaterialDynamic->SetVectorParameterValue("ShieldColor", ShieldColors[PlayerLevel]);
			}
			else
			{
				ShieldMaterialDynamic->SetVectorParameterValue("ShieldColor", FLinearColor::Blue);

			}
			UE_LOG(LogTemp, Warning, TEXT("Updating Shield Color to: %s"), *ShieldColors[PlayerLevel].ToString());

			LastPlayerLevel = PlayerLevel;
		}
	}
}

void UpHUD::UpdateRecallPB(float CurrentTime)
{
	RecallProgressBar->SetPercent(CurrentTime);
}

void UpHUD::ShowRecallBar()
{
	RecallProgressBar->SetVisibility(ESlateVisibility::Visible);
}

void UpHUD::HideRecallBar()
{
	RecallProgressBar->SetVisibility(ESlateVisibility::Collapsed);

}

void UpHUD::PerformFleshFlash()
{
	FleshFlash->SetVisibility(ESlateVisibility::Visible);
	FTimerHandle TH;
}

void UpHUD::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(GameTimeUpdateTimer);

	Super::NativeDestruct();
}

