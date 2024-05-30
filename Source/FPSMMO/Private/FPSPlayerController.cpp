// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"

#include "FPSGameState.h"
#include "MainHUD.h"
#include "PlayerHUD.h"
#include "PlayerInfoState.h"
#include "FPSCharacter.h"
#include "pHUD.h"
#include "Weapon.h"
#include "CaptureRing.h"
#include "ItemSpawnPoint.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

AFPSPlayerController::AFPSPlayerController()
{

	bReplicates = true;
}


void AFPSPlayerController::SetPlayerState(APlayerInfoState* PlayerInfoState)
{
	PIS = PlayerInfoState;
}

void AFPSPlayerController::ClientUpdateRespawnTime_Implementation(float RespawnTimeLeft)
{
	APlayerHUD* PlayerHUD = Cast<APlayerHUD>(GetHUD());
	if (PlayerHUD)
	{
		PlayerHUD->UpdateRespawnTime(RespawnTimeLeft);
	}
}
void AFPSPlayerController::ClientUpdateBountyUI_Implementation(const FText& NewText)
{
	APlayerHUD* PlayerHUD = Cast<APlayerHUD>(GetHUD());
	if (PlayerHUD)
	{
		PlayerHUD->UpdateGameInfoText(NewText);
	}
}

void AFPSPlayerController::ClientUpdateScoreboard_Implementation()
{
	APlayerHUD* PlayerHUD = Cast<APlayerHUD>(GetHUD());
	if (PlayerHUD)
	{
		PlayerHUD->RefreshScoreboard();
	}
}


void AFPSPlayerController::OnHUDReady(APlayerHUD* playerHUD)
{
	if(playerHUD)
	{
		if (AFPSGameState* GS = Cast<AFPSGameState>(GetWorld()->GetGameState()))
		{
			UE_LOG(LogTemp, Warning, TEXT("OnHUDReady Called"));
			GS->OnPlayersChanged.AddDynamic(playerHUD, &APlayerHUD::RefreshScoreboard);
		}
	}
}

void AFPSPlayerController::OnMainMenuReady(AMainHUD* MainMenuHUD)
{
	//can remove this??
}

void AFPSPlayerController::SetMenuVisibility(ESlateVisibility VisibilityType)
{
	APlayerHUD* playerHUD = Cast<APlayerHUD>(GetHUD());

	if(playerHUD)
	{
		playerHUD->SetMenuVisibility(VisibilityType);

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerHUD failed"));
	}
}

void AFPSPlayerController::ClientHideFleshFlash_Implementation()
{
	if(IsLocalController())
	{
		if (APlayerHUD* playerHUD = Cast<APlayerHUD>(GetHUD())) {
			if (playerHUD->HUDWidget) {
				playerHUD->HUDWidget->HideFleshFlash();
			}
		}
	}
}

void AFPSPlayerController::ClientUpdateShieldHUD_Implementation(float ShieldValue)
{
	if (IsLocalPlayerController()) {
		if (APlayerHUD* playerHUD = Cast<APlayerHUD>(GetHUD())) {
			if (playerHUD->HUDWidget) {
				playerHUD->HUDWidget->WidgetUpdateShield(ShieldValue);
			}
		}
	}
	
}

void AFPSPlayerController::ClientPerformFleshFlash_Implementation()
{
	if(IsLocalController())
	{
		if(APlayerHUD*playerHUD = Cast<APlayerHUD>(GetHUD()))
		{
			if(playerHUD->HUDWidget)
			{
				playerHUD->HUDWidget->PerformFleshFlash();
			}
		}
	}
}

void AFPSPlayerController::ClientUpdateShieldColor_Implementation()
{
	if (IsLocalController()) {
		if (APlayerHUD* playerHUD = Cast<APlayerHUD>(GetHUD())) {
			if (playerHUD->HUDWidget) {
				playerHUD->HUDWidget->UpdateShieldColor();
			}
		}
	}
}

void AFPSPlayerController::UpdatePostProcess_Implementation(float value)
{
	if(IsLocalController())
	{
		if(AFPSCharacter*localPlayer = Cast<AFPSCharacter>(GetPawn()))
		{
			if(localPlayer->GetCamera())
			{
				localPlayer->GetCamera()->PostProcessSettings.ColorSaturation = FVector4(value, value, value, 1.f);
			}
		}
	}
}

void AFPSPlayerController::ClientUpdateRecallProgressBar_Implementation(float CurrentTime)
{
	if(IsLocalController())
	{
		if (APlayerHUD* playerHUD = Cast<APlayerHUD>(GetHUD())) {
			if(playerHUD->HUDWidget)
			{
				playerHUD->HUDWidget->UpdateRecallPB(CurrentTime);
			}
		}
	}
}

void AFPSPlayerController::ClientShowRecallBar_Implementation()
{
	if (IsLocalController())
	{
		if (APlayerHUD* playerHUD = Cast<APlayerHUD>(GetHUD())) {
			if (playerHUD->HUDWidget)
			{
				playerHUD->HUDWidget->ShowRecallBar();
			}
		}
	}
}

void AFPSPlayerController::ClientHideRecallBar_Implementation()
{
	if (IsLocalController())
	{
		if (APlayerHUD* playerHUD = Cast<APlayerHUD>(GetHUD())) {
			if (playerHUD->HUDWidget)
			{
				playerHUD->HUDWidget->HideRecallBar();
			}
		}
	}
}

void AFPSPlayerController::ClientPlayCaptureSound_Implementation()
{
	if (AFPSGameState* GS = Cast<AFPSGameState>(GetWorld()->GetGameState())) {
		if (GS->CaptureSound) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), GS->CaptureSound, GetPawn()->GetActorLocation());
		}
	}
}


void AFPSPlayerController::ClientGlowTower_Implementation(ACaptureRing* Tower, ETeam TowerTeam, ETeam PlayerTeam)
{
	if (TowerTeam != PlayerTeam)
	{
		//this is green
		Tower->FlagMesh->SetCustomDepthStencilValue(1);
		return;
	}

		//this is red
		Tower->FlagMesh->SetCustomDepthStencilValue(2);


	
}

void AFPSPlayerController::ServerChangeTeam_Implementation(ETeam NewTeam)
{
	if (APlayerInfoState* pis = Cast<APlayerInfoState>(PlayerState))
	{
		pis->TeamId = NewTeam;
	}
}

bool AFPSPlayerController::ServerChangeTeam_Validate(ETeam NewTeam)
{
	return true;
}

void AFPSPlayerController::ChangeTeam(ETeam NewTeam)
{
	ServerChangeTeam(NewTeam);
}

void AFPSPlayerController::SetWeaponStencil_Implementation(AWeapon* Weapon)
{
	if(Weapon && Weapon->GetSpawnPoint())
	{
		if (APlayerInfoState* pis = Cast<APlayerInfoState>(GetPawn()->GetPlayerState())) {
			//if we can pick up the weapon, set the stencil to green. For some reason this works on the listen server but not the client??
			if (pis->TeamId == Weapon->GetSpawnPoint()->SpawnedTeam)
			{
				Weapon->WeaponMesh->SetCustomDepthStencilValue(1);
			}
		}
	}
}

void AFPSPlayerController::PlayCaptureRingSpawned_Implementation()
{
	if (AFPSGameState* GS = Cast<AFPSGameState>(GetWorld()->GetGameState())) {
		UGameplayStatics::PlaySound2D(this, GS->SpawnedSound);
	}
}

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

}
