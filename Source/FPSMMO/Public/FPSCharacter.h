// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

class ABombSite;
class ABomb;
class AFPSGameState;
class UPlayerCharacterMovementComponent;
struct FInputActionValue;
class APlayerInfoState;
class AFPSPlayerController;
class UpHUD;
class AWeapon;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;

UENUM(BlueprintType)
enum class EBombInteractionType : uint8
{
	None UMETA(DisplayName = "None"),
	Planting UMETA(DisplayName = "Planting"),
	Defusing UMETA(DisplayName = "Defusing")
};


UCLASS()
class FPSMMO_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Player Mesh")
		USkeletalMeshComponent* thirdPersonPlayerMesh;
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

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStartFire(FTransform SocketTransform);
	void ServerStartFire_Implementation(FTransform SocketTransform);
	bool ServerStartFire_Validate(FTransform SocketTransform);

	void Fire(FTransform SocketTransform);

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Shield")
	float MaxShield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHP;
	//Player Info
	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	float GetHealth();

	UFUNCTION(BlueprintCallable, Category = "PlayerInfo")
	float GetShield();

	void UpdateHealth();
	void UpdateShield(float SP);

	void EquipWeaponOnServer(AWeapon* Weapon);
	void EquipWeapon(AWeapon* WeaponToEquip);
	void UseWeapon(AWeapon* WeaponToUse);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerEquipWeapon(AWeapon* WeaponToEquip);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastOnWeaponEquipped(AWeapon* WeaponToEquip);



	AWeapon* GetCurrentWeapon();

	class APlayerHUD* GetPlayerHUD();

	UPROPERTY()
		UpHUD* HUDWidget;

	void SetCombatStatus(AController*EventInstigator);

	//weapon pick up
	bool bInCollision;

	UPROPERTY(Replicated)
	AWeapon* WeaponCollided;

	void WeaponSwap(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void SwitchWeapon(float Direction);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStartSprint();

	AWeapon* FindWeaponInOverlap(AWeapon* WeaponToEquip);
	

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

	UFUNCTION(BlueprintCallable, Category = "ADS")
	bool getADS();

	void UpdateWeaponTransform(float DeltaTime);

	bool CanFire();

	void StartReload(const FInputActionValue& InputActionValue);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDropWeapon(AWeapon* DroppedWeapon);
		void ServerDropWeapon_Implementation(AWeapon* DroppedWeapon);
		bool ServerDropWeapon_Validate(AWeapon* DroppedWeapon);

		UFUNCTION(NetMulticast, Reliable)
			void MulticastDropWeapon(AWeapon* DroppedWeapon);
			void MulticastDropWeapon_Implementation(AWeapon* DroppedWeapon);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
		FTransform ADSWeaponTransform = FTransform(FQuat::Identity, FVector(0, 0, 0), FVector(1, 1, 1));


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
		FTransform DefaultWeaponTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
		float ADSInterpSpeed;

	UPROPERTY(EditAnywhere, Category = "Spawn")
		TSubclassOf<AWeapon> SpawningWeapon;

	UPROPERTY(BlueprintReadWrite, Category = "Slide")
		float GroundSlope;

	void SetCanPlant(bool bCanPlant);

	bool GetCanPlant();

	FString GetKey(const FString& ActionName);

	void OpenTaccom(const FInputActionValue& InputActionValue);

	//planting
	bool bIsMovementIgnored = false;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetBombInteraction(EBombInteractionType InteractionType);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerBombInteraction();

	void StopPlanting();

	FTimerHandle BombInteractionTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "planting time")
	float BombInteractionTime;

	UPROPERTY(Replicated,VisibleAnywhere, BlueprintReadOnly, Category = "Bomb Interaction")
		EBombInteractionType BombInteractionType;

	//end planting

	//defuse


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "bomb")
		TSubclassOf<ABomb>bomb;

	//spawning

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawnWeapon(TSubclassOf<AWeapon>WeaponClass);


	FTimerHandle DestructionTimer;
	float TimeBeforeDestroy = 2.f;

	void SetDeathEXP(int32 NewDeathEXP);

	void SetDeathGold(int32 NewDeathGold);

	int32 GetDeathEXP();

	int32 GetDeathGold();


private:
	UPROPERTY(ReplicatedUsing = OnRep_Sliding)
		bool bIsSliding;

	UPROPERTY(Replicated,EditAnywhere, Category = "PlayerInfo")
		int32 DeathEXP;

	UPROPERTY(Replicated, EditAnywhere, Category = "PlayerInfo")
		int32 DeathGold;

	float MinimumSlideSpeed = 1500.0f;


	float DefaultFOV;

	float TargetControllerRecoil;

	float DownwardKick;
	//planting
	UPROPERTY(Replicated)
	bool CanPlant;

	AFPSGameState* GS;


	bool bCanOpenMarket;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Health")
		virtual float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	// First-person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class UCameraComponent* FPSCameraComponent;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "WeaponMesh")
		class USkeletalMeshComponent* WeaponMesh;

	// First-person mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		class USkeletalMeshComponent* FPSMeshArms;

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

	UPROPERTY(VisibleAnywhere,Replicated,BlueprintReadOnly, Category = "Weapons")
		TArray<AWeapon*> EquippedWeapons;


	AWeapon* WeapToEquip;

	UPROPERTY(EditDefaultsOnly, Category = "pHUDClass")
		TSubclassOf<UpHUD> pHUDClass;


	UFUNCTION()
		void OnRep_CurrentWeapon();

	UPROPERTY()
		AFPSPlayerController* FPSPC;

	UFUNCTION(NetMulticast, Reliable)
		void SpawnProjectile(FVector SpawnLocation, FRotator SpawnRotation);
		void SpawnProjectile_Implementation(FVector SpawnLocation, FRotator SpawnRotation);
		

	AProjectile* Projectile;

	FTransform GetFiringPosition();
	void DestroyCharacter();
	void KilledBy(AController* EventInstigator);

	void Interact();

	UFUNCTION(Server, Reliable)
		void HandleInteract();

	bool IsButtonPressed();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
			UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
		class UInputConfigData* InputActions;


	UPROPERTY(ReplicatedUsing = OnRep_Sprint)
		bool bSprint;

	UFUNCTION()
		void OnRep_Sprint() const;

	void ApplyRecoil();

	void HandleReloadProgress(float DeltaTime);

	float ReloadProgress = 0.0f;

	FRotator NormalWeaponRotation;
	UPROPERTY(EditAnywhere, Category = "Weapon Info")
	FRotator WeaponTargetRotation;

	FRotator OriginalSocketRotation;

	bool bIsFiring;

	virtual void DetachFromControllerPendingDestroy() override;

	virtual void PossessedBy(AController* NewController) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ServerTick(float DeltaTime);

	void StartADS(const FInputActionValue& InputActionValue);
	void ManageScoreboard(const FInputActionValue& InputActionValue);
	bool bScoreboardOpen;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Player Info
	void SetHealth(float hp);
	void SetShield(float shield);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetHealth(float val);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetShield(float val);

	UFUNCTION()
		void OnRep_Health();
	UFUNCTION()
		void OnRep_Shield();

	void SetCanOpenMarket(bool val);

	bool CanOpenMarket();
};
