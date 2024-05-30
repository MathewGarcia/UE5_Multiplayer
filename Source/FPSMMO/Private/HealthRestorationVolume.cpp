// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthRestorationVolume.h"

#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "PlayerInfoState.h"

// Sets default values
AHealthRestorationVolume::AHealthRestorationVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	Box->SetGenerateOverlapEvents(true);
	RootComponent = Box;
	SetReplicates(true);
}

void AHealthRestorationVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		if(AFPSCharacter*player = Cast<AFPSCharacter>(OtherActor))
		{
			if(APlayerInfoState*PS = Cast<APlayerInfoState>(player->GetPlayerState()))
			{
				if(PS->TeamId == Team)
				{
					FTimerHandle TimerHandle;
					GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this,player]()
					{
							if (HasAuthority()) {
								if (player->GetHealth() != player->MaxHP) {
									player->SetHealth(player->GetHealth() + 10);
								}
							}

					},0.5f,true);
					RechargeHealthTimers.Add(player, TimerHandle);
				}
			}
		}
	}
}

void AHealthRestorationVolume::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		if(AFPSCharacter*player = Cast<AFPSCharacter>(OtherActor))
		{
			if(RechargeHealthTimers.Contains(player))
			{
				GetWorld()->GetTimerManager().ClearTimer(RechargeHealthTimers[player]);
				RechargeHealthTimers.Remove(player);
			}
		}
	}
}

// Called when the game starts or when spawned
void AHealthRestorationVolume::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this, &AHealthRestorationVolume::OnOverlapBegin);
}

// Called every frame
void AHealthRestorationVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

