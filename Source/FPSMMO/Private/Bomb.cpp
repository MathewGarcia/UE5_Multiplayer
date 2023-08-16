// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomb.h"
#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "PlayerHUD.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#define LOCTEXT_NAMESPACE "Gameplay"
// Sets default values
ABomb::ABomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bomb Mesh"));
	RootComponent = BombMesh;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->InitBoxExtent(FVector(100.f));
	BoxComponent->SetVisibility(true);
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ABomb::OnBoxBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ABomb::OnBoxEndOverlap);

	ExplosionEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ExplosionEffect"));
	ExplosionEffect->SetupAttachment(RootComponent);



	// Set the explosion effect to the explosion particle system
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionEffectAsset(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (ExplosionEffectAsset.Succeeded())
	{
		ExplosionEffect->SetTemplate(ExplosionEffectAsset.Object);
	}

	ExplosionEffect->bAutoActivate = false;

	DefusingEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DefusingEffect"));
	DefusingEffect->SetupAttachment(RootComponent);
	DefusingEffect->bAutoActivate = false;
}


void ABomb::PlaceAtLocation(const FVector& Location)
{
	FVector TraceEnd = Location - FVector(0, 0, 100.0f);
	
	FCollisionQueryParams QueryParams;

	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;

	FHitResult Hit;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Location, TraceEnd, ECC_Visibility))
	{
		SetActorLocation(Hit.Location);
	}
	DrawDebugLine(GetWorld(), Location, TraceEnd, FColor::Red, true, 1, 0, 2);

}

void ABomb::Explode()
{
	ExplosionEffect->ActivateSystem();
	if(ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
}

void ABomb::MulticastExplode_Implementation()
{
	Explode();
}

void ABomb::MulticastDefusing_Implementation()
{
	DefusingEffect->ActivateSystem();
}

void ABomb::SetTeam(ETeam TeamID)
{
	TeamId = TeamID;
}

ETeam ABomb::GetTeam()
{
	return TeamId;
}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABomb::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		AFPSCharacter* player = Cast<AFPSCharacter>(OtherActor);
		if (player)
		{
			SetOwner(player);
		
				FString Key = player->GetKey("IA_Interact");
				FText text = FText::Format(LOCTEXT("IA_Interact", "Press {0} to Defuse"), FText::FromString(Key));

				if (player->IsLocallyControlled()) {
					APlayerHUD* PlayerHUD = player->GetPlayerHUD();
					if (PlayerHUD) {
						PlayerHUD->UpdateText(text);
					}
				}
		}
	}
}
#undef LOCTEXT_NAMESPACE

void ABomb::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{

	if (OtherActor)
	{
		AFPSCharacter* player = Cast<AFPSCharacter>(OtherActor);
		if (player && player->IsLocallyControlled())
		{

			APlayerHUD* PlayerHUD = player->GetPlayerHUD();
			if (PlayerHUD) {
				PlayerHUD->UpdateText(FText::FromString(""));
			}
		}
	}
}

// Called every frame
void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABomb::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABomb, TeamId);
}


