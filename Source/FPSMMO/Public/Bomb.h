// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bomb.generated.h"

class UBoxComponent;
UCLASS()
class FPSMMO_API ABomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABomb();

	UPROPERTY(EditAnywhere, Category = "Static Mesh")
		UStaticMeshComponent* BombMesh;

	UPROPERTY(EditAnywhere, Category = "Box Component")
	UBoxComponent* BoxComponent;

	void PlaceAtLocation(const FVector&Location);

	UPROPERTY(EditAnywhere, Category = "Bomb Info")
		float BombTimer;


	UPROPERTY(EditAnywhere, Category = "Effects")
		class UParticleSystemComponent* ExplosionEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
		class UParticleSystemComponent* PlantingEffect;
	UPROPERTY(EditAnywhere, Category = "Effects")
		class UParticleSystemComponent* DefusingEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
		USoundWave* ExplosionSound;
	UPROPERTY(EditAnywhere, Category = "Effects")
		USoundWave* DefusingSound;

	void Explode();

	UFUNCTION(NetMulticast,Reliable)
		void MulticastExplode();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastDefusing();
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
