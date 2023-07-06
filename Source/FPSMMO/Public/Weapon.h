// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Weapon.generated.h"

class UBoxComponent;
UCLASS()
class FPSMMO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* BoxCollision;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float Damage;

	UPROPERTY(EditAnywhere, Category = "WeaponInfo")
		float WeaponDistance;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "WeaponMesh")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapon")
		float FireRate;


	UFUNCTION()
		void OnRep_PickedUp();

	bool GetPickedUp();

	void SetPickUp(bool isPickedUp);

	UPROPERTY(Replicated)
	bool bIsAttached = false;

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnBoxEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Server, Reliable)
	void KillActor();

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Recoil")
		float MaxVerticalRecoil;

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Recoil")
		float MaxHorizontalRecoil;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bIsAutomatic;

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int AmmoInClip;
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int MaxAmmo;
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int MaxAmmoInClip;

	UPROPERTY(ReplicatedUsing = OnRep_Reloading,EditAnywhere,BlueprintReadWrite, Category = "Weapon")
		bool bIsReloading;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerResetReload();

	UFUNCTION()
		void OnRep_Reloading();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStartReload();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void MulticastStartReload();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetPickUp(bool bSetPickUp);
		void ServerSetPickUp_Implementation(bool bSetPickUp);
		bool ServerSetPickUp_Validate(bool bSetPickUp);



	void StartReload();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FTimerHandle ReloadTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float ReloadSpeed;

	void ResetReload();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool bIsShotgun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int ShotgunPellets;

protected:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,ReplicatedUsing = OnRep_PickedUp, Category = "Weapon Info")
		bool bPickedUp;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
