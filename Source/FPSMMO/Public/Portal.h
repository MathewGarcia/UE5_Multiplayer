// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

class AFPSCharacter;
class UBoxComponent;

UCLASS()
class FPSMMO_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

	UPROPERTY(EditAnywhere, Category = "Mesh")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere,Category = "Box")
	UBoxComponent* Box;


	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere,Category = "TeleportLink")
	TSoftObjectPtr<APortal> ConnectedPortal;

	
	UFUNCTION(Server,Reliable,WithValidation)
	void ServerTeleport(AFPSCharacter* SendingPlayer);

	void Teleport(AFPSCharacter* SendingPlayer);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCanTeleport(bool CanTP);

	void SetCanTeleport(bool CanTP);

	UPROPERTY(EditAnywhere,Replicated,Category = "CanTeleport")
	bool bCanTeleport;


	FTimerHandle DeactivationHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
