// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoState.h"
#include "GameFramework/Actor.h"
#include "CaptureRing.generated.h"

class AWeapon;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRingPointsUpdated);
class UBoxComponent;
class AFPSGameState;

enum class ETeam : uint8;

UENUM(BlueprintType)
enum class EPos : uint8 {
	EPos_Front UMETA(DisplayName = "Front"),
	EPos_Back UMETA(DisplayName = "Back")
};

UENUM(BlueprintType)
enum class ELane : uint8 {
	ELane_Top UMETA(DisplayName = "Top"),
	ELane_Mid UMETA(DisplayName = "Mid"),
	ELane_Bot UMETA(DisplayName = "Bot")
};
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
		TSubclassOf<AWeapon>WeaponToDrop;

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnBoxEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Ring Info")
	int GetRingPoints();

	FTimerHandle RingTimerHandle;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerIncrementPoints();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDropWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerHandleOverlap(AFPSCharacter* Player);

	void DropWeapon();

	void IncrementRingPoints();

	UFUNCTION(BlueprintCallable, Category = "Ring Info")
	int GetMaxRingPoints();

	void SetMaxRingPoints(int32 NewMaxRingPoints);

	UFUNCTION()
	void OnRep_RingPoints();

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnRingPointsUpdated OnRingPointsUpdated;

	UFUNCTION()
	void OnRep_DropWeapon();

	bool IsCaptured();

	UFUNCTION()
	void OnRep_Captured();

	UPROPERTY(EditAnywhere, Category = "Position")
	EPos pos;

	UPROPERTY(EditAnywhere, Category = "Lane")
	ELane lane;

private:

	UPROPERTY(ReplicatedUsing=OnRep_RingPoints)
	int RingPoints;

	UPROPERTY(EditAnywhere, Category = "Ring Info")
		int MaxRingPoints;

	UPROPERTY(ReplicatedUsing = "OnRep_DropWeapon")
		bool bIsWeaponDropped;

	UPROPERTY(ReplicatedUsing = "OnRep_Captured")
		bool bIsCaptured;

	AFPSGameState* GS;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
