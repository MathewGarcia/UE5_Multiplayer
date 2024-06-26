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
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerInfoState.h"
#include "Engine/StaticMeshActor.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Engine/DamageEvents.h"
#include "FPSMMO/FPSMMOGameModeBase.h"


class AGameState;
class UGameplayStatics;
// Sets default values
//TODO GET TYPE OF GRENADE
AProjectile::AProjectile()
{
	SetReplicateMovement(true);

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
	ProjectileMovementComponent->InitialSpeed = 1000.0f;
	ProjectileMovementComponent->MaxSpeed = 9999.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
	ProjectileMovementComponent->ProjectileGravityScale = 1.0f;
	ProjectileMovementComponent->bIsHomingProjectile = false;
	

	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}


		ExplosionForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("Radial Force Comp"));
		ExplosionForce->SetupAttachment(RootComponent);
		ExplosionForce->Radius = 500.f;
		ExplosionForce->ImpulseStrength = 100000.f;
		ExplosionForce->bAutoActivate = false;

}

UProjectileMovementComponent* AProjectile::GetProjectileMovement()
{
	return ProjectileMovementComponent;
}

void AProjectile::MulticastSpawnBlood_Implementation(FVector Location, AFPSCharacter* Victim)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Victim->BloodParticle, Location, FRotator::ZeroRotator);

}

FVector AProjectile::CalcImpactPoint(AStaticMeshActor* StaticMeshActor)
{
	FVector GrenadeLocation = GetActorLocation();
	FVector MeshLocation = StaticMeshActor->GetActorLocation();

	FVector ImpactPoint = MeshLocation + (MeshLocation - GrenadeLocation).GetSafeNormal() * ExplosionForce->Radius;
	return ImpactPoint;
}


void AProjectile::SetFiringPlayer(AFPSCharacter* FP)
{
	FiringPlayer = FP;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Set a timer to destroy the projectile after a certain amount of time
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AProjectile::OnDestroyTimerExpired, DestroyDelay, false);
	

	CollisionComponent->IgnoreActorWhenMoving(this, true);
	CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);

	
	if(GetInstigator())
	{
		GetInstigator()->MoveIgnoreActorAdd(GetInstigator());
	}

	if (bIsGrenade) {
		GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AProjectile::Explode, ExplosionTime, false);
	}
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

		// Reduce the max speed over time
		ProjectileMovementComponent->MaxSpeed = FMath::Max(ProjectileMovementComponent->MaxSpeed - (DeltaTime * SpeedDecreaseRate), 0.0f);

		if (ProjectileMovementComponent->MaxSpeed <= 0.0f)
		{
			Destroy();
		}

}

void AProjectile::FireInDirection(const FVector& Direction)
{
	// Set the projectile's initial velocity
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
}

void AProjectile::SetFiredWeapon(AWeapon* Weapon)
{
	FiredWeapon = Weapon;
}

void AProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if (bIsGrenade) return;

	// Check if the actor that was hit is a player
	AFPSCharacter* player = Cast<AFPSCharacter>(GetOwner());
	APlayerInfoState* PlayerInfoState = Cast<APlayerInfoState>(player->GetPlayerState());
	if (player && PlayerInfoState)
	{
		AWeapon* Weapon = player->GetCurrentWeapon();
		if (Weapon) {
			if (OtherActor) {
				AFPSCharacter* EnemyPlayer = Cast<AFPSCharacter>(OtherActor);
				if(player == EnemyPlayer)
				{
					return;
				}
				if (EnemyPlayer) {

					if (AFPSMMOGameModeBase* GM = Cast<AFPSMMOGameModeBase>(GetWorld()->GetAuthGameMode())) {
						//if friendly fire is not on check the team, else we can just deal damage.
						if (!GM->bFF) {
							APlayerInfoState* EnemyPlayerState = Cast<APlayerInfoState>(EnemyPlayer->GetPlayerState());
							if (EnemyPlayerState && EnemyPlayerState->TeamId != PlayerInfoState->TeamId)
							{
								EnemyPlayer->HitResults.Add(Hit);
								EnemyPlayer->TakeDamage(Weapon->Damage, FDamageEvent(), player->GetController(), player);
								MulticastSpawnBlood(Hit.Location, EnemyPlayer);
							}
						}
						else
						{
							EnemyPlayer->HitResults.Add(Hit);
							EnemyPlayer->TakeDamage(Weapon->Damage, FDamageEvent(), player->GetController(), player);
							MulticastSpawnBlood(Hit.Location, EnemyPlayer);

							//UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EnemyPlayer->BloodParticle, Hit.Location, FRotator::ZeroRotator);
						}
					
					}
				}
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

	if(isExplosive)
	{
		Explode();
		if (FiredWeapon)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FiredWeapon->FiringSound, Hit.Location);
			UGameplayStatics::ApplyRadialDamage(this, FiredWeapon->Damage, Hit.Location, ExplosionForce->Radius,UDamageType::StaticClass(), TArray<AActor*>(), this, GetOwner()->GetInstigatorController(), true, ECollisionChannel::ECC_Visibility);

			UE_LOG(LogTemp, Warning, TEXT("FiredWeapon is NOT null"));

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FiredWeapon is null"));
		}
	}

	Destroy();
}

void AProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CollisionComponent->IgnoreActorWhenMoving(this, true);
	CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);


}

void AProjectile::Destroyed()
{
	Super::Destroyed();

}

void AProjectile::Explode()
{

	ExplosionForce->FireImpulse();

	FVector spawnLocation = GetActorLocation();

	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);

	


}

void AProjectile::OnDestroyTimerExpired()
{
	// Destroy the projectile
	Destroy();
}
