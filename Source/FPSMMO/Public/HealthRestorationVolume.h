// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "HealthRestorationVolume.generated.h"

class AFPSCharacter;
enum class ETeam : uint8;

UCLASS()
class FPSMMO_API AHealthRestorationVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealthRestorationVolume();

	UPROPERTY(EditAnywhere, Category = "BoxComponent")
	UBoxComponent* Box;

	UPROPERTY(EditAnywhere, Category = "Team")
	 ETeam Team;

	 UFUNCTION()
	  void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	  UFUNCTION()
	  void OnComponentEndOverlap(
		  UPrimitiveComponent* OverlappedComp,
		  AActor* OtherActor,
		  UPrimitiveComponent* OtherComp,
		  int32 OtherBodyIndex
	  );

	  TMap<AFPSCharacter*, FTimerHandle> RechargeHealthTimers;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
