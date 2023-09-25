// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class AMainMenuGameStateBase;
class AFPSGameState;
class UListView;
class UOverlay;
class UButton;
/**
 * 
 */
UCLASS()
class FPSMMO_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
		UMainMenuWidget(const FObjectInitializer& ObjectInitializer);

		UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UOverlay* StartingScreenOverlay;

		UPROPERTY(BlueprintReadWrite,meta = (BindWidget))
		UOverlay* SelectionScreenOverlay;

			UPROPERTY(VisibleAnywhere, meta = (BindWidget))
			UButton* SelectionButton;

		UPROPERTY(meta = (BindWidget))
		UButton* HostSelectionButton;

		UPROPERTY(meta = (BindWidget))
			UListView* PlayerListView;

		UPROPERTY(BlueprintReadWrite,meta = (BindWidget))
			UOverlay* LobbyOverlay;

		UPROPERTY(BlueprintReadWrite,VisibleAnywhere,meta = (BindWidget))
			UButton* StartButton;


		bool bIsInitialScreen = true;

	UFUNCTION()
		void FindGame();


	UFUNCTION()
		void HostGame();

	UFUNCTION()
		void HandlePlayerJoin(APlayerController* Player);

	UFUNCTION()
		void StartGame();

	UPROPERTY(BlueprintReadOnly, Category = "FPSGameState")
		AMainMenuGameStateBase* GS;

	void SetupWidget();
	UFUNCTION()
	void HandleUI();

protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;


	virtual void NativeConstruct() override;
	
};
