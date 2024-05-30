// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class AWeapon;
class AFPSCharacter;
class UpHUD;
class UProjectileMovementComponent;
class URadialForceComponent;
class AStaticMeshActor;
UCLASS()
class FPSMMO_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	// Function to get the projectile's collision component
	class USphereComponent* GetCollisionComponent() const { return CollisionComponent; }


	UPROPERTY(EditAnywhere, Category = "Effects")
		class UParticleSystem* ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Info")
		float SpeedDecreaseRate;

	UProjectileMovementComponent* GetProjectileMovement();

	bool bIsShotgunPellet = false;

	UPROPERTY(EditAnywhere, Category = "IsGrenade")
	bool bIsGrenade = false;

		UPROPERTY(EditAnywhere, Category = "Explosive")
	bool isExplosive;


	
	UPROPERTY(VisibleAnywhere, Category = "Explosion")
		URadialForceComponent* ExplosionForce;


		UFUNCTION(NetMulticast, Reliable)
		void MulticastSpawnBlood(FVector Location, AFPSCharacter* Victim);


	FVector CalcImpactPoint(AStaticMeshActor* StaticMeshActor);

	void SetFiringPlayer(AFPSCharacter* FP);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Category = "Projectile")
		void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void PostInitializeComponents() override;

	virtual void Destroyed() override;

	void Explode();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void FireInDirection(const FVector& Direction);
	void SetFiredWeapon(AWeapon* Weapon);

private:
	// Projectile movement component
	UPROPERTY(VisibleAnywhere, Category = "Movement")
		UProjectileMovementComponent* ProjectileMovementComponent;

	// Projectile collision component
	UPROPERTY(VisibleAnywhere, Category = "Collision")
		USphereComponent* CollisionComponent;

	// Timer handle for destroying the projectile
	FTimerHandle DestroyTimerHandle;

	// Delay before destroying the projectile
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		float DestroyDelay = 10.0f;

	AFPSCharacter* FiringPlayer;

	AWeapon* FiredWeapon;

	FTimerHandle ExplosionTimerHandle;

	float ExplosionTime = 3.0f;
	// Function to destroy the projectile
	void OnDestroyTimerExpired();


};
