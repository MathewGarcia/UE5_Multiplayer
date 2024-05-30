// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"

#include "FPSCharacter.h"
#include "FPSGameState.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	Box->SetGenerateOverlapEvents(true);
	Box->SetBoxExtent(FVector(50.f));
	Box->SetupAttachment(StaticMesh);

	bCanTeleport = true;

}

void APortal::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		if (AFPSCharacter* player = Cast<AFPSCharacter>(OtherActor))
		{
			if (bCanTeleport) {
				if (HasAuthority()) {
					SetOwner(player);
					Teleport(player);
				}
				else {
					ServerTeleport(player);
				}
			}
		}
	}
}

void APortal::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	bool bIsPortalInArray = false;
	bool bTimerActive = false;

	if(AFPSGameState*GS = Cast<AFPSGameState>(GetWorld()->GetGameState()))
	{
		for(TSoftObjectPtr<APortal>port : GS->Portals)
		{
			if(this == port.Get())
			{
				bIsPortalInArray = true;
				bTimerActive = GetWorld()->GetTimerManager().IsTimerActive(DeactivationHandle);
				break;
			}
		}
	}

	if (!bIsPortalInArray || bTimerActive) {
		ServerCanTeleport(true);
	}
}


void APortal::ServerTeleport_Implementation(AFPSCharacter* SendingPlayer)
{
	Teleport(SendingPlayer);
}

bool APortal::ServerTeleport_Validate(AFPSCharacter* SendingPlayer)
{
	return true;
}

void APortal::Teleport(AFPSCharacter* SendingPlayer)
{
	if (HasAuthority()) {
		if (SendingPlayer && bCanTeleport)
		{
				if (APortal* TargetPortal = ConnectedPortal.Get()) {
					UE_LOG(LogTemp, Warning, TEXT("APortal::Teleport - Teleporting to: %s"), *TargetPortal->GetName());
					TargetPortal->bCanTeleport = false;
					SendingPlayer->SetActorLocation(TargetPortal->GetActorLocation());
				}
			
		}
	}
	else
	{
		ServerTeleport(SendingPlayer);
	}
}

void APortal::ServerCanTeleport_Implementation(bool CanTP)
{
	SetCanTeleport(CanTP);
}

bool APortal::ServerCanTeleport_Validate(bool CanTP)
{
	return true;
}

void APortal::SetCanTeleport(bool CanTP)
{
	if (HasAuthority()) {
		bCanTeleport = CanTP;
	}
	else
	{
		ServerCanTeleport(CanTP);
	}
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnBoxBeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &APortal::OnBoxEndOverlap);

	if (ConnectedPortal.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("APortal::BeginPlay - ConnectedPortal is valid: %s"), *ConnectedPortal.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APortal::BeginPlay - ConnectedPortal is not valid"));
	}
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APortal, bCanTeleport);
}