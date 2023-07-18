// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoState.h"
#include "GameFramework/Actor.h"
#include "CaptureRing.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRingPointsUpdated);
class UBoxComponent;
enum class ETeam : uint8;

UCLASS()
class FPSMMO_API ACaptureRing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACaptureRing();

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
		ETeam TeamId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Box Collision")	
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		USkeletalMeshComponent* FlagMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
		float TimerRate;

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnBoxEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Ring Info")
	int GetRingPoints();

	FTimerHandle RingTimerHandle;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerIncrementPoints();

	void IncrementRingPoints();

	UFUNCTION(BlueprintCallable, Category = "Ring Info")
	int GetMaxRingPoints();


	UFUNCTION()
	void OnRep_RingPoints();

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnRingPointsUpdated OnRingPointsUpdated;


private:

	UPROPERTY(ReplicatedUsing=OnRep_RingPoints)
	int RingPoints;

	UPROPERTY(EditAnywhere, Category = "Ring Info")
		int MaxRingPoints;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
