// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Weapon.generated.h"

class UBoxComponent;
UCLASS()
class FPSMMO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();


	UBoxComponent* BoxCollision;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float Damage;

	UPROPERTY(EditAnywhere, Category = "WeaponInfo")
		float WeaponDistance;

	UPROPERTY(EditAnywhere, Category = "WeaponMesh")
	class UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapon")
		float FireRate;


	UFUNCTION()
		void OnRep_PickedUp();

	bool GetPickedUp();

	void SetPickUp(bool isPickedUp);

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnBoxEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,ReplicatedUsing = OnRep_PickedUp, Category = "Weapon Info")
		bool bPickedUp;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
