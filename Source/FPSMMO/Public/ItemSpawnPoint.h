// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnPoint.generated.h"

class AWeapon;

UCLASS()
class FPSMMO_API AItemSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSpawnPoint();

	UPROPERTY(EditAnywhere, Category = "Weapon To Spawn")
		TSubclassOf<AWeapon>WeaponToSpawn;

	UPROPERTY()
		AWeapon* SpawnedWeapon;

	UPROPERTY(EditAnywhere,Category = "Respawn Time")
	float RespawnTime;

	void SpawnItem();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawnItem();

	void OnWeaponPickedUp();

private:
	FTimerHandle RespawnTimer;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
