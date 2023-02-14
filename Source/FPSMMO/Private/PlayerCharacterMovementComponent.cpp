// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterMovementComponent.h"

#include "Net/UnrealNetwork.h"

UPlayerCharacterMovementComponent::UPlayerCharacterMovementComponent()
{
	bIsSliding = false;
}

void UPlayerCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsSliding)
	{
		Velocity = Velocity.GetSafeNormal() * 1500.0f;
	}
}

bool UPlayerCharacterMovementComponent::GetSliding()
{
	return bIsSliding;
}


void UPlayerCharacterMovementComponent::OnRep_Sliding()
{
	SetSliding();
}

bool UPlayerCharacterMovementComponent::CanSlide()
{
	if(IsMovingOnGround() && Velocity.Size() > 1000.0f &&bIsSliding == false)
	{
		return true;
	}
	return false;
}

void UPlayerCharacterMovementComponent::SetSliding()
{
	if(GetOwner()->HasAuthority())
	{
		bIsSliding = !bIsSliding;
	}
}

void UPlayerCharacterMovementComponent::ServerSlide_Implementation()
{
	SetSliding();
	OnRep_Sliding();
}

bool UPlayerCharacterMovementComponent::ServerSlide_Validate()
{
	return true;
}

void UPlayerCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate variables to all clients
	DOREPLIFETIME(UPlayerCharacterMovementComponent, bIsSliding);
}
