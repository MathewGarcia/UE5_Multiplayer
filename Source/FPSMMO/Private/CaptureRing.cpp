// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureRing.h"

#include "FPSCharacter.h"
#include "TeamEnum.h"
#include "Components/BoxComponent.h"

// Sets default values
ACaptureRing::ACaptureRing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
			GetWorld()->GetTimerManager().SetTimer(RingTimerHandle, this, &ACaptureRing::IncrementRingPoints, TimerRate, true);
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

void ACaptureRing::IncrementRingPoints()
{
	if (RingPoints < MaxRingPoints) {
		RingPoints++;
		UE_LOG(LogTemp, Warning, TEXT("Incrementing points: %d "), RingPoints);

	}
}

// Called when the game starts or when spawned
void ACaptureRing::BeginPlay()
{
	Super::BeginPlay();

	RingPoints = 0;
	
}

// Called every frame
void ACaptureRing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

