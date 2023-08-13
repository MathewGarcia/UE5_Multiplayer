// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureRing.h"

#include "FPSCharacter.h"
#include "TeamEnum.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACaptureRing::ACaptureRing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	FlagMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Flag Mesh"));
	RootComponent = FlagMesh;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetupAttachment(FlagMesh);
	BoxComponent->InitBoxExtent(FVector(200.f));
	BoxComponent->SetVisibility(true);
	BoxComponent->SetGenerateOverlapEvents(true);

	TeamId = ETeam::TEAM_NONE;

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACaptureRing::OnBoxBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ACaptureRing::OnBoxEndOverlap);
}

void ACaptureRing::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFPSCharacter* player = Cast<AFPSCharacter>(OtherActor);
	if(player)
	{
		APlayerInfoState* PlayerInfoState = Cast<APlayerInfoState>(player->GetPlayerState());
		if(PlayerInfoState && PlayerInfoState->TeamId != this->TeamId)
		{
			if (HasAuthority()) {
				GetWorld()->GetTimerManager().SetTimer(RingTimerHandle, this, &ACaptureRing::IncrementRingPoints, TimerRate, true);
			}
			UE_LOG(LogTemp, Warning, TEXT("IN RING"));
		}
	}
}

void ACaptureRing::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetWorld()->GetTimerManager().TimerExists(RingTimerHandle)) {
		GetWorld()->GetTimerManager().ClearTimer(RingTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("TIMER CLEARED"));
	}

}

int ACaptureRing::GetRingPoints()
{
	return RingPoints;
}

void ACaptureRing::ServerIncrementPoints_Implementation()
{
	IncrementRingPoints();
}

bool ACaptureRing::ServerIncrementPoints_Validate()
{
	return true;
}

void ACaptureRing::IncrementRingPoints()
{
	if (RingPoints < MaxRingPoints && HasAuthority()) {
		RingPoints++;
		UE_LOG(LogTemp, Warning, TEXT("Incrementing points: %d "), RingPoints);
		if(RingPoints == MaxRingPoints)
		{
			//TODO:update the exp and gold.
		}
	}
}

int ACaptureRing::GetMaxRingPoints()
{
	return MaxRingPoints;
}


void ACaptureRing::OnRep_RingPoints()
{
	OnRingPointsUpdated.Broadcast();
}

// Called when the game starts or when spawned
void ACaptureRing::BeginPlay()
{
	Super::BeginPlay();

	RingPoints = 0;
	
}

void ACaptureRing::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACaptureRing, RingPoints);
}

// Called every frame
void ACaptureRing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

