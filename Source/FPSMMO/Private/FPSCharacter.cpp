// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
#include "Engine.h"
#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneComponent.h"
#include "PlayerHUD.h"
#include "pHUD.h"
#include "FPSPlayerController.h"
#include "PlayerInfoState.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{

	SetReplicates(true);
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
		// Set size for collision capsule
		GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

		// Create a follow camera
		FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
		FPSCameraComponent->SetupAttachment(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
		FPSCameraComponent->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
		
		// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
		FPSMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSMesh"));
		FPSMeshComponent->SetOnlyOwnerSee(true);
		FPSMeshComponent->SetupAttachment(FPSCameraComponent);
		FPSMeshComponent->bCastDynamicShadow = false;
		FPSMeshComponent->CastShadow = false;

		WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
		WeaponMesh->SetupAttachment(FPSCameraComponent);
		WeaponMesh->SetOnlyOwnerSee(true);


		playerMeshTest = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("playerMesh"));
		playerMeshTest->SetupAttachment(RootComponent);
		// Make sure the static mesh is visible
		playerMeshTest->SetVisibility(true);
		playerMeshTest->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		playerMeshTest->SetOnlyOwnerSee(false);
		// Disable crouching
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = false;

		MaxShield = 100;

}

void AFPSCharacter::SetCombatStatus(AController* EventInstigator)
{
	if (EventInstigator) {
		APlayerInfoState* instigatorPlayerState = Cast<APlayerInfoState>(EventInstigator->PlayerState);
		if (instigatorPlayerState)
		{
			if (instigatorPlayerState->GetInCombat() != true)
				instigatorPlayerState->OnRep_InCombat();
		}
	}

	APlayerInfoState* PS = Cast<APlayerInfoState>(GetPlayerState());

	if (PS)
	{
		if(PS->GetInCombat() != true)
		PS->OnRep_InCombat();
	}
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	bCanFire = true;

	UObject* Asset = LoadObject<UObject>(nullptr, TEXT("/Game/PlayerStuff/BP_TestWeapon.BP_TestWeapon"));
	if (Asset)
	{
		if (Asset->IsA<UBlueprint>())
		{
			UBlueprint* BP = Cast<UBlueprint>(Asset);
			if (BP->GeneratedClass && BP->GeneratedClass->IsChildOf(AWeapon::StaticClass()))
			{
				// Create an instance of the weapon object
				AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(BP->GeneratedClass);
				EquipWeapon(Weapon);
				UseWeapon(Weapon);
			}
		
		}
	}


	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		FPSPC = Cast<AFPSPlayerController>(PC);
	}

	if (FPSPC->IsLocalPlayerController())
	{
		HUDWidget = FPSPC->HUDWidget;
	}

	SetShield(100);
	SetHealth(100);

}

void AFPSCharacter::SetHealth(float hp)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		HP = hp;
		OnRep_Health();
	}

}

void AFPSCharacter::SetShield(float shield)
{
	if (GetLocalRole() == ROLE_Authority) {
		Shield = FMath::Clamp(shield, 0.0f, MaxShield);
		OnRep_Shield();
	}
	
}

void AFPSCharacter::OnRep_Health()
{
	UpdateHealth();
}

void AFPSCharacter::OnRep_Shield()
{
	UpdateShield(Shield);
}

void AFPSCharacter::OnRep_CurrentWeapon()
{
	CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CurrentWeapon->AttachToComponent(playerMeshTest, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Weapon");
	CurrentWeapon->SetOwner(this);
	CurrentWeapon->WeaponMesh->SetOwnerNoSee(true);
}

void AFPSCharacter::HandleFire_Implementation()
{

	FRotator SpawnRotation = GetControlRotation();

		if (CurrentWeapon) {
			// Set the projectile's initial velocity
			if (Projectile)
			{
				FVector LaunchDirection = SpawnRotation.Vector();
				LaunchDirection *= CurrentWeapon->WeaponDistance;
				Projectile->FireInDirection(LaunchDirection);


				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("FIRING"));

			}

		}
}

void AFPSCharacter::SpawnProjectile_Implementation()
{

	// Spawn a projectile at the camera location
	FVector SpawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 100.0f) + (GetActorUpVector() * 50.0f);
	FRotator SpawnRotation = GetControlRotation();
	FActorSpawnParameters spawnParameters;

	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, spawnParameters);
}

void AFPSCharacter::KilledBy(AController* EventInstigator)
{
	if (EventInstigator) {
		APlayerInfoState* instigatorPlayerState = Cast<APlayerInfoState>(EventInstigator->PlayerState);
		if (instigatorPlayerState)
		{
			instigatorPlayerState->UpdateEXP(10);
		}
	}
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("RotateCameraYaw", this, &AFPSCharacter::RotateCameraYaw);
	PlayerInputComponent->BindAxis("RotateCameraPitch", this, &AFPSCharacter::RotateCameraPitch);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPSCharacter::StopFire);
}

void AFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// Add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// Add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

// Handle camera rotation input
void AFPSCharacter::RotateCameraYaw(float Value)
{
	if (Value != 0.0f)
	{
		AddControllerYawInput(Value);
	}
}

void AFPSCharacter::RotateCameraPitch(float Value)
{
	if (Value != 0.0f)
	{
		AddControllerPitchInput(Value);
	}
}

void AFPSCharacter::StartFire()
{
	//TODO:Change firerate to weapon->FireRate
	if (bCanFire) {
		bCanFire = false;
		// Set a timer to allow firing again
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AFPSCharacter::ResetFire, FireRate, false);
		SpawnProjectile();
	}
}

void AFPSCharacter::StopFire()
{
	// Clear the fire timer
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);

	// Reset the firing flag
	bCanFire = true;
}

void AFPSCharacter::ResetFire()
	{
		bCanFire = true;
	}

float AFPSCharacter::GetHealth()
{
	return HP;
}

float AFPSCharacter::GetShield()
{
	return Shield;
}
void AFPSCharacter::UpdateHealth()
{
	if (IsLocallyControlled()) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Health: " + FString::Printf(TEXT("%f"), GetHealth())));

		}

		if (FPSPC)
		{
			float NHP = GetHealth();
			FPSPC->UpdateHP(NHP);
		}

		if(GetHealth() <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("You are Dead."));
		
		}
	}
	
}

void AFPSCharacter::UpdateShield(float SP)
{
	if (IsLocallyControlled()) {

		Shield = SP;

		if (GEngine) {

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("You have  " + FString::Printf(TEXT("%f shield remaining"), GetShield())));

		}


		if (FPSPC)
		{
			float NSP = GetShield();
			FPSPC->UpdateShield(NSP);
		}
	}
	if (GetLocalRole() == ROLE_Authority) {
	
	}
}

void AFPSCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
		EquippedWeapons.Add(WeaponToEquip);
		if (!CurrentWeapon)
		{
			CurrentWeapon = WeaponToEquip;
		}
}



void AFPSCharacter::UseWeapon(AWeapon* Weapon)
{

		CurrentWeapon = Weapon;
		OnRep_CurrentWeapon();
}

void AFPSCharacter::SetClientSideWeapon(AWeapon* Weapon)
{
	if (Weapon)
	{
		//set the weapon mesh
		WeaponMesh = Weapon->WeaponMesh;
	}
}

AWeapon* AFPSCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

APlayerHUD* AFPSCharacter::GetPlayerHUD()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->ClientSetHUD(APlayerHUD::StaticClass());
		APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PC->GetHUD());
		return PlayerHUD;
	}
	return nullptr;
}
float AFPSCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
		if (GetShield() > 0) {
			float damageApplied = GetShield() - DamageTaken;
			SetShield(damageApplied);
			SetCombatStatus(EventInstigator);
			return damageApplied;
		}
		else
		{
			float damageApplied = GetHealth() - DamageTaken;
			SetHealth(damageApplied);
			if(GetHealth() <= 0)
			{
				KilledBy(EventInstigator);
			}
			return damageApplied;
		}

	
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate variables to all clients
	DOREPLIFETIME(AFPSCharacter, HP);
	DOREPLIFETIME(AFPSCharacter, Shield);
	DOREPLIFETIME(AFPSCharacter, CurrentWeapon);
}
