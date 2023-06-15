// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

class UPlayerCharacterMovementComponent;
struct FInputActionValue;
class APlayerInfoState;
class AFPSPlayerController;
class UpHUD;
class AWeapon;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;

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

	//possibly save for later?
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

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStartSprint();

	void SetSprint(bool &bSprintVal, bool Val);

	bool GetSprint();

	void StartSprint();
	//crouch
	void StartCrouch();

	void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Crouch)
		FVector CrouchEyeOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Crouch)
		float CrouchSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		UPlayerCharacterMovementComponent* PlayerCharacterMovement;


	UFUNCTION(BlueprintCallable, Category = Slide)
		bool GetSliding();

	UFUNCTION()
		void OnRep_Sliding();

	bool CanSlide();

	void SetSliding(bool bInSliding);
	void ApplySlide();
	void AdjustCameraForSlide(bool bInSliding, float DeltaTime);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSlide();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerEndSlide();

	void EndSlide();

	UPROPERTY(Replicated, BlueprintReadWrite, Category = SlideTime)
	float SlideStartTime;

	UPROPERTY(Replicated)
		AController* LastDamagingPlayer;

	float CrouchedHalfHeight = 40.0f;
	float StandingHalfHeight = 88.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float CameraInterpSpeed = 10.f;

	void PerformSlide(float DeltaTime);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPerformSlide(float DeltaTime);

	UPROPERTY(Replicated, BlueprintReadWrite, Category = Slide)
	FVector InitialSlideVelocity;

	UPROPERTY(ReplicatedUsing = OnRep_IsADS)
		bool bIsADS;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetADS(bool NewADS);
	void ServerSetADS_Implementation(bool NewADS);
	bool ServerSetADS_Validate(bool NewADS);

	void UpdateCameraFOV();

	void ADS();

	void SetADS(bool Val);

	UFUNCTION()
	void OnRep_IsADS();

	bool getADS();

	void UpdateWeaponTransform(float DeltaTime);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
		FTransform ADSWeaponTransform = FTransform(FQuat::Identity, FVector(0, 0, 0), FVector(1, 1, 1));


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
		FTransform DefaultWeaponTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
		float ADSInterpSpeed;
private:
	UPROPERTY(ReplicatedUsing = OnRep_Sliding)
		bool bIsSliding;


	float MinimumSlideSpeed = 1500.0f;

	float GroundSlope;

	float DefaultFOV;



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
			UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
		class UInputConfigData* InputActions;


	UPROPERTY(ReplicatedUsing = OnRep_Sprint)
		bool bSprint;

	UFUNCTION()
		void OnRep_Sprint();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ServerTick(float DeltaTime);

	void StartADS(const FInputActionValue& InputActionValue);
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
