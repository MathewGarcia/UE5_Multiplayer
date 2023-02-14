// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class FPSMMO_API UPlayerCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

		UPlayerCharacterMovementComponent();
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = Slide)
	bool GetSliding();

	UFUNCTION()
		void OnRep_Sliding();

	bool CanSlide();

	void SetSliding();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSlide();
private:
	UPROPERTY(ReplicatedUsing=OnRep_Sliding)
	bool bIsSliding;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;



};
