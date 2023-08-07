// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarketActor.generated.h"

class UMarketDataAsset;
class UBoxComponent;
UCLASS()
class FPSMMO_API AMarketActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMarketActor();

	UPROPERTY(EditAnywhere, Category = "Box Component")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, Category = "Actor Mesh")
	UStaticMeshComponent* ActorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UMarketDataAsset* MarketDataAsset;

	void SetupMarket();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnBoxEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
