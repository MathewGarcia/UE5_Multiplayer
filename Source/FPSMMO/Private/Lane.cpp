// Fill out your copyright notice in the Description page of Project Settings.


#include "Lane.h"
#include "TeamEnum.h"
#include "Net/UnrealNetwork.h"
#include "CaptureRing.h"


ALane::ALane()
{
	bReplicates = true;
}

void ALane::Delete(ACaptureRing* ringToDelete)
{
	//root node would not be able to be destroyed since the ACaptureRing* would be null. Which is good we don't want to destroy the root node?

	ALane* current = this;
	ALane* parent = nullptr;

	while (current && ringToDelete != current->ring) {

		parent = current;
		if (ringToDelete->TeamId == ETeam::TEAM_RED && current->LChild != nullptr) {
			current = current->LChild;
		}
		else if (ringToDelete->TeamId == ETeam::TEAM_BLUE && current->RChild != nullptr) {
			current = current->RChild;
		}
	}

	if (current == nullptr) {
		return;
	}

		if (current->LChild == nullptr && current->RChild == nullptr) {
			if (parent->LChild == current) parent->LChild = nullptr;
			else if (parent->RChild == current) parent->RChild = nullptr;
			else {
				this->ring = nullptr;
			}
		}
		if (current && current->ring) {
			current->ring->Destroy();
			current->ConditionalBeginDestroy();
		}
}

void ALane::Insert(ACaptureRing*ringToAdd)
{
	ALane* NewLane = NewObject<ALane>();
	NewLane->ring = ringToAdd;

	ALane* current = this;
	while (current) {
		if (ringToAdd->TeamId == ETeam::TEAM_RED) {
			if (current->LChild == nullptr) {
				current->LChild = NewLane;
				break;
			}
			current = current->LChild;
		}
		else {
			if (current->RChild == nullptr) {
				current->RChild = NewLane;
				break;
			}
			current = current->RChild;
		}
	}
}

bool ALane::CanCapture(ACaptureRing* currentRing)
{
	ALane* current = this;
	int safetyCounter = 0;

	while (current)
	{
		if (current->ring == currentRing)
		{
			// If both are true, return true; else, it will return false.
			return current->LChild == nullptr && current->RChild == nullptr;
		}

		// Determine direction to go based on the team color
		ALane* nextNode = (currentRing->TeamId == ETeam::TEAM_RED) ? current->LChild : current->RChild;

		if (!nextNode) {
			UE_LOG(LogTemp, Warning, TEXT("Next node is null, breaking out of loop"));
			break;
		}

		current = nextNode;

		if (++safetyCounter > 3) // Arbitrary large number
		{
			UE_LOG(LogTemp, Warning, TEXT("LANE NOT FOUND"));
			break;
		}
	}
	return false;
}

void ALane::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALane, RChild);
	DOREPLIFETIME(ALane, LChild);
}


