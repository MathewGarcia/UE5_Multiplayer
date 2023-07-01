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
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetupAttachment(RootComponent);
	SetReplicates(true);
	WeaponMesh->SetOwnerNoSee(true);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComp"));
	BoxCollision->SetupAttachment(WeaponMesh);

	bReplicates = true;
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

void AWeapon::ResetReload()
{
	bIsReloading = false;
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

void AWeapon::ServerResetReload_Implementation()
{
	ResetReload();
}

bool AWeapon::ServerResetReload_Validate()
{
	return true;
}

void AWeapon::OnRep_Reloading()
{
	
}

void AWeapon::ServerStartReload_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ServerStartReload called"));

	MulticastStartReload();
}

bool AWeapon::ServerStartReload_Validate()
{
	return true;
}

void AWeapon::MulticastStartReload_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("MulticastStartReload called"));

	StartReload();
}

bool AWeapon::MulticastStartReload_Validate()
{
	return true;
}

void AWeapon::StartReload()
{

	UE_LOG(LogTemp, Warning, TEXT("StartReload called"));
	if (!HasAuthority())
		return;

	if (bIsReloading || AmmoInClip == MaxAmmoInClip)
	{
		// Already reloading or clip is full
		return;
	}

	bIsReloading = true;
	// Calculate how much ammo we need to fill the clip
	int AmmoNeeded = MaxAmmoInClip - AmmoInClip;

	// Calculate how much ammo to actually move from reserves to the clip
	int AmmoToMove = FMath::Min(AmmoNeeded, MaxAmmo);

	// Adjust ammo counts
	AmmoInClip += AmmoToMove;
	MaxAmmo -= AmmoToMove;

	// Clear the existing timer if it is already running
	if (GetWorld()->GetTimerManager().IsTimerActive(ReloadTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	}

	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &AWeapon::ServerResetReload, ReloadSpeed, false);
}


void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate MyReplicatedVariable to all clients
	DOREPLIFETIME(AWeapon, Damage);
	DOREPLIFETIME(AWeapon, bPickedUp);
	DOREPLIFETIME(AWeapon, MaxVerticalRecoil);
	DOREPLIFETIME(AWeapon, MaxHorizontalRecoil);
	DOREPLIFETIME(AWeapon, AmmoInClip);
	DOREPLIFETIME(AWeapon, MaxAmmo);
	DOREPLIFETIME(AWeapon,MaxAmmoInClip);
	DOREPLIFETIME(AWeapon, bIsReloading);
	DOREPLIFETIME(AWeapon, bIsAttached);
}
