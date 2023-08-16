// Fill out your copyright notice in the Description page of Project Settings.


#include "MarketActor.h"

#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "MarketWidget.h"
#include "PlayerHUD.h"
#include "Components/BoxComponent.h"


#define LOCTEXT_NAMESPACE "Gameplay"


// Sets default values
AMarketActor::AMarketActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Actor Mesh"));
	RootComponent = ActorMesh;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->InitBoxExtent(FVector(100.f));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AMarketActor::OnBoxBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AMarketActor::OnBoxEndOverlap);

}

void AMarketActor::SetupMarket()
{
	if (GetWorld()) {
		AFPSCharacter* player = Cast<AFPSCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (player && player->GetPlayerHUD())
		{
			player->GetPlayerHUD()->UpdateMarket(MarketDataAsset);
		}
	}


}


// Called when the game starts or when spawned
void AMarketActor::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle Timer;

	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AMarketActor::SetupMarket, 2.f,false);
}

void AMarketActor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		AFPSCharacter* player = Cast<AFPSCharacter>(OtherActor);
		if(player && player->IsLocallyControlled())
		{
			player->SetCanOpenMarket(true);
			
				FString Key = player->GetKey("IA_Interact");
				FText text = FText::Format(LOCTEXT("IA_Interact", "Press {0} to Open Market"), FText::FromString(Key));
				APlayerHUD* PlayerHUD = player->GetPlayerHUD();
				if (PlayerHUD) {
					PlayerHUD->UpdateText(text);
				}
			
		}
	}
}
#undef LOCTEXT_NAMESPACE


void AMarketActor::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		AFPSCharacter* player = Cast<AFPSCharacter>(OtherActor);

		if(player && player->IsLocallyControlled())
		{
			player->SetCanOpenMarket(false);
			APlayerHUD* PlayerHUD = player->GetPlayerHUD();
			if (PlayerHUD) {
				PlayerHUD->UpdateText(FText::FromString(""));
			}

		}
	}
}

// Called every frame
void AMarketActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

