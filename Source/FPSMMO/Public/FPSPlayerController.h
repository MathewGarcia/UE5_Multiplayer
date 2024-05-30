// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

class AWeapon;
class ACaptureRing;
enum class ETeam : uint8;
class AMainHUD;
class APlayerHUD;
class UMarketDataAsset;
class UMarketWidget;
class APlayerInfoState;
class UTaccomWidget;
class AFPSCharacter;
class UpHUD;
/**
 * 
 */
UCLASS()
class FPSMMO_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
		AFPSPlayerController();

		AFPSCharacter* player;

		void SetPlayerState(APlayerInfoState*PlayerInfoState);

		UFUNCTION(Client, Reliable)
			void ClientUpdateRespawnTime(float RespawnTimeLeft);

		UFUNCTION(Client, Reliable)
			void ClientUpdateBountyUI(const FText& NewText);

		UFUNCTION(Client, Reliable)
			void ClientUpdateScoreboard();

		void OnHUDReady(APlayerHUD* playerHUD);

		void OnMainMenuReady(AMainHUD* MainMenuHUD);

		void SetMenuVisibility(ESlateVisibility VisibilityType);

	UFUNCTION(Client,Reliable)
	void ClientUpdateShieldHUD(float ShieldValue);

	UFUNCTION(Client, Reliable)
	void ClientPerformFleshFlash();

	UFUNCTION(Client, Reliable)
	void ClientHideFleshFlash();

	UFUNCTION(Client, Reliable)
	void ClientUpdateShieldColor();

	UFUNCTION(Client, Reliable)
	void UpdatePostProcess(float value);

	UFUNCTION(Client, Reliable)
	void ClientUpdateRecallProgressBar(float CurrentTime);

	UFUNCTION(Client, Reliable)
	void ClientShowRecallBar();

	UFUNCTION(Client,Reliable)
	void ClientHideRecallBar();

	UFUNCTION(Client, Reliable)
	void ClientPlayCaptureSound();

	UFUNCTION(Client, Reliable)
	void ClientGlowTower(ACaptureRing*Tower,ETeam TowerTeam, ETeam PlayerTeam);

	UFUNCTION(Exec,Category = "Commands")
	void ChangeTeam(ETeam NewTeam);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeTeam(ETeam NewTeam);

	UFUNCTION(Client, Reliable)
	void SetWeaponStencil(AWeapon*Weapon);

	UFUNCTION(Client, Reliable)
	void PlayCaptureRingSpawned();

protected:

	virtual void BeginPlay() override;


private:
	UPROPERTY()
	APlayerInfoState* PIS;


	
};
