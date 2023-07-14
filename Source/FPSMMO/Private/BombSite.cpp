// Fill out your copyright notice in the Description page of Project Settings.

#include "BombSite.h"
#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "Components/BoxComponent.h"

#define LOCTEXT_NAMESPACE "Gameplay"


ABombSite::ABombSite()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>("Bomb Mesh");
	RootComponent = BombMesh;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->InitBoxExtent(FVector(100.f));
	BoxComponent->SetupAttachment(BombMesh);
	BoxComponent->SetVisibility(true);
	BoxComponent->SetGenerateOverlapEvents(true);

	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ABombSite::OnBoxBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ABombSite::OnBoxEndOverlap);
}

// Called when the game starts or when spawned
void ABombSite::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABombSite::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		AFPSCharacter* player = Cast<AFPSCharacter>(OtherActor);
		if(player)
		{
			player->SetCanPlant(true);
			AFPSPlayerController*PC= Cast<AFPSPlayerController>(player->GetController());
			if(PC)
			{
				FString Key = player->GetKey("IA_Interact");
				FText text = FText::Format(LOCTEXT("IA_Interact", "Press {0} to Plant"), FText::FromString(Key));

				PC->UpdateText(text);
			}
		}
	}
}

void ABombSite::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AFPSCharacter* player = Cast<AFPSCharacter>(OtherActor);
		if (player)
		{
			player->SetCanPlant(false);
			AFPSPlayerController* PC = Cast<AFPSPlayerController>(player->GetController());
			if (PC)
			{
			

				PC->UpdateText(FText::FromString(TEXT("")));
			}
		}
	}
}

// Called every frame
void ABombSite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#undef LOCTEXT_NAMESPACE