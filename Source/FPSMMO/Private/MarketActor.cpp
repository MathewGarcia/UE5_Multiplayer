// Fill out your copyright notice in the Description page of Project Settings.


#include "MarketActor.h"

#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "MarketWidget.h"
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
	AFPSPlayerController* PC = Cast<AFPSPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		UE_LOG(LogTemp, Error, TEXT("Set up market"));
		PC->UpdateMarket(MarketDataAsset);
	}


}


// Called when the game starts or when spawned
void AMarketActor::BeginPlay()
{
	Super::BeginPlay();

	SetupMarket();
}

void AMarketActor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		AFPSCharacter* player = Cast<AFPSCharacter>(OtherActor);
		if(player)
		{
			player->SetCanOpenMarket(true);
			AFPSPlayerController* PC = Cast<AFPSPlayerController>(player->GetController());
			if (PC)
			{
				FString Key = player->GetKey("IA_Interact");
				FText text = FText::Format(LOCTEXT("IA_Interact", "Press {0} to Open Market"), FText::FromString(Key));
				PC->UpdateText(text);
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

		if(player)
		{
			player->SetCanOpenMarket(false);
			AFPSPlayerController* PC = Cast<AFPSPlayerController>(player->GetController());
			if (PC)
			{
				PC->UpdateText(FText::FromString(""));
			}
		}
	}
}

// Called every frame
void AMarketActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

