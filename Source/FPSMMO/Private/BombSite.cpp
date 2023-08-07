// Fill out your copyright notice in the Description page of Project Settings.

#include "BombSite.h"
#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "FPSGameState.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "Gameplay"


ABombSite::ABombSite()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>("Bomb Mesh");
	RootComponent = BombMesh;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->InitBoxExtent(FVector(100.f));
	BoxComponent->SetupAttachment(BombMesh);
	BoxComponent->SetVisibility(true);
	BoxComponent->SetGenerateOverlapEvents(true);

	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ABombSite::OnBoxBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ABombSite::OnBoxEndOverlap);

	bBoxComponentEnabled = true;
}

void ABombSite::OnRep_BoxComponentEnabled()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_BoxComponentEnabled called"));
	BoxComponent->SetGenerateOverlapEvents(bBoxComponentEnabled);
}

void ABombSite::SetPlantedBomb(ABomb* Bomb)
{
	PlantedBomb = Bomb;
}

ABomb* ABombSite::GetPlantedBomb()
{
	return PlantedBomb;
}

// Called when the game starts or when spawned
void ABombSite::BeginPlay()
{
	Super::BeginPlay();

	AFPSGameState* GS = GetWorld()->GetGameState<AFPSGameState>();
	if (GS)
	{
		GS->BombSites.Add(this);
	}
}

void ABombSite::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning,TEXT("Begin Overlap bombsite called"))
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
#undef LOCTEXT_NAMESPACE

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

void ABombSite::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Here, "bBoxComponentEnabled" is the name of the replicated property.
	// Replace this with the actual name of your replicated property.
	DOREPLIFETIME(ABombSite, bBoxComponentEnabled);
	DOREPLIFETIME(ABombSite, PlantedBomb);
}