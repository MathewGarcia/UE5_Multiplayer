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

	SetShield(100);
	SetHealth(100);

	OnRep_FPSController();

}

void AFPSCharacter::SetHealth(float hp)
{
	
		HP = hp;
	
}

void AFPSCharacter::SetShield(float shield)
{
		Shield = shield;
		
	
}

void AFPSCharacter::OnRep_Health()
{
	UpdateHealth();
}

void AFPSCharacter::OnRep_Shield()
{
	UpdateShield();
}

void AFPSCharacter::OnRep_FPSController()
{

		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
			FPSPC = Cast<AFPSPlayerController>(PC);
		if (FPSPC)
			UE_LOG(LogTemp, Warning, TEXT("FPSPC Good"));
	
}

void AFPSCharacter::OnRep_CurrentWeapon()
{
	CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CurrentWeapon->AttachToComponent(playerMeshTest, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Weapon");
	CurrentWeapon->SetOwner(this);
	CurrentWeapon->WeaponMesh->SetOwnerNoSee(true);
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


	if (bCanFire)
	{
		bCanFire = false;

		// Spawn a projectile at the camera location
		FVector SpawnLocation = FPSCameraComponent->GetComponentLocation();
		FRotator SpawnRotation = FPSCameraComponent->GetComponentRotation();
		FActorSpawnParameters spawnParameters;

		spawnParameters.Instigator = GetInstigator();
		spawnParameters.Owner = this;

		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation,spawnParameters);


		if (CurrentWeapon) {
			// Set the projectile's initial velocity
			if (Projectile)
			{
				FVector LaunchDirection = SpawnRotation.Vector();
				LaunchDirection *= CurrentWeapon->WeaponDistance;
				Projectile->FireInDirection(LaunchDirection);

			
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("FIRING"));

			}

			// Set a timer to allow firing again
			GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AFPSCharacter::ResetFire, FireRate, false);
		}
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
	
	if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Health: " + FString::Printf(TEXT("%f"), GetHealth())));
		
	}

	if (FPSPC)
	{
		FPSPC->UpdateHP();
	}
	
}

void AFPSCharacter::UpdateShield()
{

		if (GEngine) {

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Shield: " + FString::Printf(TEXT("%f"), GetShield())));

		}

	


		if (FPSPC)
		{
			FPSPC->UpdateShield(GetShield());
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
		//Weapon->AttachToComponent(FPSCameraComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
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
	if (GetShield() >= 0) {
		float damageApplied = GetShield() - DamageTaken;
		SetShield(damageApplied);
		APlayerState*PS = this->GetPlayerState();
		if(PS)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s, is taking damage!"), *PS->GetName());
			FPSPC = Cast<AFPSPlayerController>(this->GetWorld()->GetFirstPlayerController());
			if(FPSPC)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s, is the player controller"), *FPSPC->GetName());

				OnRep_Shield();
			}

		}
		return damageApplied;
	}
	else
	{
		float damageApplied = GetHealth() - DamageTaken;
		SetHealth(damageApplied);
		OnRep_Health();
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
	DOREPLIFETIME(AFPSCharacter, FPSPC);
}
