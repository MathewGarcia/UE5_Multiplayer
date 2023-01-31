// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "Components/BoxComponent.h"
// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetupAttachment(RootComponent);
	SetReplicates(true);
	WeaponMesh->SetOwnerNoSee(true);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComp"));
	BoxCollision->SetupAttachment(WeaponMesh);

	bPickedUp = false;
}

void AWeapon::OnRep_PickedUp()
{
	SetPickUp(bPickedUp);
}

bool AWeapon::GetPickedUp()
{
	return bPickedUp;
}

void AWeapon::SetPickUp(bool isPickedUp)
{
	if(GetLocalRole() == ROLE_Authority)
	bPickedUp = isPickedUp;
}

void AWeapon::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFPSCharacter* player = Cast<AFPSCharacter>(OtherActor);
	if(player)
	{
		AFPSPlayerController* FPSPC = Cast<AFPSPlayerController>(player->GetController());
		if(FPSPC)
		{
			FText text = FText::FromString(TEXT("Press E to Pick Up"));

			FPSPC->UpdateText(text);
		}
		player->bInCollision = true;
		player->WeaponCollided = this;
	}
}

void AWeapon::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{

	AFPSCharacter* player = Cast<AFPSCharacter>(OtherActor);
	if (player)
	{
		AFPSPlayerController* FPSPC = Cast<AFPSPlayerController>(player->GetController());
		if (FPSPC)
		{
			FText text = FText::FromString(TEXT(""));
			FPSPC->UpdateText(text);
		}

		player->WeaponCollided = nullptr;
		player->bInCollision = false;
	}
}

void AWeapon::KillActor_Implementation()
{
	Destroy();
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxBeginOverlap);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnBoxEndOverlap);

}


// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bPickedUp)
	{
		BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

}
void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate MyReplicatedVariable to all clients
	DOREPLIFETIME(AWeapon, Damage);
	DOREPLIFETIME(AWeapon, bPickedUp);
}
