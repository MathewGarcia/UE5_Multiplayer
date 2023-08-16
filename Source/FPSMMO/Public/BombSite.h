// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombSite.generated.h"

enum class ETeam : uint8;
class ABomb;
class UBoxComponent;
UCLASS()
class ABombSite : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABombSite();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Box Collision")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb Mesh")
		UStaticMeshComponent* BombMesh;

	UPROPERTY(ReplicatedUsing = OnRep_BoxComponentEnabled)
		bool bBoxComponentEnabled;

	UFUNCTION()
		void OnRep_BoxComponentEnabled();

	void SetPlantedBomb(ABomb*Bomb);

	ABomb* GetPlantedBomb();

	void SetTeam(ETeam NewTeam);

	ETeam GetTeam();
private:
	UPROPERTY(Replicated)
		ABomb* PlantedBomb;

	UPROPERTY(Replicated, EditAnywhere, Category= "Bomb Team")
		ETeam Team;

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
