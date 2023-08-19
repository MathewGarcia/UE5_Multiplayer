

// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawnPoint.h"
#include "Net/UnrealNetwork.h"
#include "Weapon.h"

// Sets default values
AItemSpawnPoint::AItemSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AItemSpawnPoint::SpawnItem()
{
	if (HasAuthority()) {
		SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponToSpawn, GetActorLocation(), GetActorRotation());
		if (SpawnedWeapon) {
			SpawnedWeapon->SetSpawnPoint(this);
		}
	}
}


void AItemSpawnPoint::ServerSpawnItem_Implementation()
{
	SpawnItem();
}

bool AItemSpawnPoint::ServerSpawnItem_Validate()
{
	return true;
}

void AItemSpawnPoint::OnWeaponPickedUp()
{
	GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this ,&AItemSpawnPoint::ServerSpawnItem, RespawnTime, false);
}

// Called when the game starts or when spawned
void AItemSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		SpawnItem();
	}
}

// Called every frame
void AItemSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

