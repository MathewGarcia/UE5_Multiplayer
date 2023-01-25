// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

struct FInputActionValue;
class APlayerInfoState;
class AFPSPlayerController;
class UpHUD;
class AWeapon;
class USpringArmComponent;
class UCameraComponent;
UCLASS()
class FPSMMO_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Player Mesh")
		UStaticMeshComponent* playerMeshTest;
public:
	// Sets default values for this character's properties
	AFPSCharacter();

	// Handle movement input
	void Move(const FInputActionValue& Value);

	// Handle camera rotation input
	void Look(const FInputActionValue& Value);

	// Handle firing input
	void StartFire(const FInputActionValue& Val);
	void StopFire();
	void ResetFire();

	float MaxShield;

	//Player Info
	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	float GetHealth();

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	float GetShield();

	void UpdateHealth();
	void UpdateShield(float SP);

	void EquipWeapon(AWeapon* WeaponToEquip);
	void UseWeapon(AWeapon* WeaponToUse);

	void SetClientSideWeapon(AWeapon* Weapon);

	AWeapon* GetCurrentWeapon();

	class APlayerHUD* GetPlayerHUD();

	UFUNCTION(BlueprintCallable, Category = "Health")
		float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	UPROPERTY()
		UpHUD* HUDWidget;

	void SetCombatStatus(AController*EventInstigator);

	//weapon pick up
	bool bInCollision;

	AWeapon* WeaponCollided;

	void WeaponSwap(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void SwitchWeapon(float Direction);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// First-person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class UCameraComponent* FPSCameraComponent;

	UPROPERTY(EditAnywhere, Category = "WeaponMesh")
		class UStaticMeshComponent* WeaponMesh;

	// First-person mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		class USkeletalMeshComponent* FPSMeshComponent;

	// Projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<class AProjectile> ProjectileClass;

	// Flag to control firing 
	uint32 bCanFire : 1;

	// Timer handle for continuous firing
	FTimerHandle FireTimerHandle;

	float FireRate = 1;

	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Health,Category = "playerInfo")
		float HP;
	UPROPERTY(EditAnywhere, ReplicatedUsing= OnRep_Shield,Category = "playerInfo")
		float Shield;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, ReplicatedUsing= OnRep_CurrentWeapon, Category = "Weapon")
		AWeapon* CurrentWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_PreviousWeapon, Category = "Weapon")
	AWeapon* PreviousWeapon = nullptr;

	UFUNCTION()
	void OnRep_PreviousWeapon();

	UPROPERTY(VisibleAnywhere,ReplicatedUsing=OnRep_EquippedWeapons,BlueprintReadOnly, Category = "Weapons")
		TArray<AWeapon*> EquippedWeapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	TArray<AWeapon*> ClientWeapons;

	UFUNCTION()
		void OnRep_EquippedWeapons();

	void UpdateEquippedWeapons();

	AWeapon* WeapToEquip;

	UPROPERTY(EditDefaultsOnly, Category = "pHUDClass")
		TSubclassOf<UpHUD> pHUDClass;


	UFUNCTION()
		void OnRep_CurrentWeapon();

	UPROPERTY()
		AFPSPlayerController* FPSPC;

	UFUNCTION(Server, Reliable)
		void HandleFire();

	UFUNCTION(Server, Reliable)
		void SpawnProjectile();

	AProjectile* Projectile;

	void KilledBy(AController* EventInstigator);

	void Interact();

	UFUNCTION(Server, Reliable)
		void HandleInteract();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
		class UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
		class UInputConfigData* InputActions;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Player Info
	void SetHealth(float hp);
	void SetShield(float shield);
	UFUNCTION()
		void OnRep_Health();
	UFUNCTION()
		void OnRep_Shield();
};
