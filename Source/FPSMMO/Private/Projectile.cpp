// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "FPSCharacter.h"
#include "pHUD.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

class AGameState;
// Sets default values
AProjectile::AProjectile()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	  // Create the collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->SetVisibility(true);
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = CollisionComponent;

	if (GetLocalRole() == ROLE_Authority)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnProjectileImpact);
	}



	// Set the projectile to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Create a projectile movement component
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(GetCollisionComponent());
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 9999.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Set a timer to destroy the projectile after a certain amount of time
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AProjectile::OnDestroyTimerExpired, DestroyDelay, false);
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::FireInDirection(const FVector& Direction)
{
	// Set the projectile's initial velocity
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
}

void AProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Check if the actor that was hit is a player
	AFPSCharacter* player = Cast<AFPSCharacter>(GetOwner());

	if (player)
	{

		AWeapon* Weapon = player->GetCurrentWeapon();
		if (Weapon) {
			if (OtherActor)
			{
				UGameplayStatics::ApplyPointDamage(OtherActor, Weapon->Damage, NormalImpulse, Hit, GetInstigator()->Controller, this, DamageType);
			}
		}
		else

		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Weapon failed"));

		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Player failed"));
	}


	Destroy();
}

void AProjectile::Destroyed()
{
	Super::Destroyed();

	FVector spawnLocation = GetActorLocation();

	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);

}

void AProjectile::OnDestroyTimerExpired()
{
	// Destroy the projectile
	Destroy();
}
