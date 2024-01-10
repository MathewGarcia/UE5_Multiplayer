// Fill out your copyright notice in the Description page of Project Settings.


#include "Lane.h"
#include "TeamEnum.h"
#include "CaptureRing.h"


void ULane::Delete(ACaptureRing* ringToDelete)
{
	//root node would not be able to be destroyed since the ACaptureRing* would be null. Which is good we don't want to destroy the root node?

	ULane* current = this;
	ULane* parent = nullptr;

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
		current->ring->Destroy();
		current->ConditionalBeginDestroy();
}

void ULane::Insert(ACaptureRing*ringToAdd)
{
	ULane* NewLane = NewObject<ULane>();
	NewLane->ring = ringToAdd;

	ULane* current = this;
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

bool ULane::CanCapture(ACaptureRing* currentRing)
{
	ULane* current = this;
	int safetyCounter = 0;

	while (current)
	{
		if (current->ring == currentRing)
		{
			// If both are true, return true; else, it will return false.
			return current->LChild == nullptr && current->RChild == nullptr;
		}

		// Determine direction to go based on the team color
		ULane* nextNode = (currentRing->TeamId == ETeam::TEAM_RED) ? current->LChild : current->RChild;

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
