// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "FPSCharacter.h"
#include "FPSGameState.h"
#include "FPSPlayerController.h"
#include "ItemSpawnPoint.h"
#include "PlayerHUD.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"

#define LOCTEXT_NAMESPACE "Gameplay"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetOwnerNoSee(true);
	WeaponMesh->SetRenderCustomDepth(true);
	RootComponent = WeaponMesh;


	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComp"));
	BoxCollision->SetupAttachment(WeaponMesh);
	BoxCollision->SetRelativeScale3D(FVector(30, 30, 30));
	BoxCollision->SetVisibility(true);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	BoxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
	BoxCollision->SetGenerateOverlapEvents(true);


	bPickedUp = false;
}

void AWeapon::OnRep_PickedUp()
{
	BoxCollision->SetGenerateOverlapEvents(!bPickedUp);
	if (!bPickedUp) {
		SetOwner(nullptr);
		SetActorHiddenInGame(false);
	}
}

bool AWeapon::GetPickedUp()
{
	return bPickedUp;
}

void AWeapon::SetPickUp(bool isPickedUp)
{
	bPickedUp = isPickedUp;
	BoxCollision->SetGenerateOverlapEvents(!bPickedUp);
	if(GetWorld()->GetTimerManager().IsTimerActive(DespawnTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(DespawnTimer);
	}

	if (!bPickedUp) {
		SetOwner(nullptr);
		SetWeaponStencil();
		SetActorHiddenInGame(false);
		
		GetWorld()->GetTimerManager().SetTimer(DespawnTimer, [this]()
		{
				Destroy();
		}, DespawnTime,false);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Set pick up is true, so %s won't be dropped"),*GetName());
		WeaponMesh->SetCustomDepthStencilValue(0);
	}
}

bool AWeapon::GetPickup()
{
	return bPickedUp;
}

void AWeapon::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFPSCharacter* player = Cast<AFPSCharacter>(OtherActor);
	if(player)
	{
	
			FString Key = player->GetKey("IA_Interact");
			FText text = FText::Format(LOCTEXT("WeaponPickUpPrompt", "Press {0} to pick up {1}"), FText::FromString(Key), FText::FromString(WeaponName));
			if (player->IsLocallyControlled()) {
				APlayerHUD* PlayerHUD = player->GetPlayerHUD();
				if (PlayerHUD) {
					PlayerHUD->UpdateText(text);
				}
			}
		player->bInCollision = true;
		player->WeaponCollided = this;
	}
}
#undef LOCTEXT_NAMESPACE
void AWeapon::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{

	AFPSCharacter* player = Cast<AFPSCharacter>(OtherActor);
	if (player)
	{
		
			FText text = FText::FromString(TEXT(""));
			if (player->IsLocallyControlled()) {
				APlayerHUD* PlayerHUD = player->GetPlayerHUD();
				if (PlayerHUD) {
					PlayerHUD->UpdateText(text);
				}
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

int32 AWeapon::GetWeaponEXP()
{
	return WeaponEXP;
}

int32 AWeapon::GetWeaponGold()
{
	return WeaponGold;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxBeginOverlap);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnBoxEndOverlap);

	if(AFPSGameState*GS = Cast<AFPSGameState>(GetWorld()->GetGameState()))
	{
		FTimerHandle TH;
		GetWorld()->GetTimerManager().SetTimer(TH, [GS,this]()
		{
			GS->SetWeaponStencil(this);
		}, 0.5f ,false);
	}
}


// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::SetWeaponStencil()
{
	if (SpawnPoint && SpawnPoint->SpawnedTeam == ETeam::TEAM_RED)
	{
		WeaponMesh->SetCustomDepthStencilValue(1);
	}
	else if (SpawnPoint && SpawnPoint->SpawnedTeam == ETeam::TEAM_BLUE)
	{
		WeaponMesh->SetCustomDepthStencilValue(2);
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

void AWeapon::ServerSetPickUp_Implementation(bool bSetPickUp)
{
	SetPickUp(bSetPickUp);
}

bool AWeapon::ServerSetPickUp_Validate(bool bSetPickUp)
{
	return true;
}

void AWeapon::SetSpawnPoint(AItemSpawnPoint* spawnPoint)
{
	SpawnPoint = spawnPoint;
}

AItemSpawnPoint* AWeapon::GetSpawnPoint()
{
	if (SpawnPoint) {
		return SpawnPoint;
	}
	return nullptr;
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
	DOREPLIFETIME(AWeapon, WeaponSpread);
	DOREPLIFETIME(AWeapon, WeaponEXP);
	DOREPLIFETIME(AWeapon, WeaponGold);
	DOREPLIFETIME(AWeapon, SpawnPoint);
}
