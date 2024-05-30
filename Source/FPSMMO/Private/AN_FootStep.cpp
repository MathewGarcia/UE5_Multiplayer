// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_FootStep.h"

#include "FPSCharacter.h"

void UAN_FootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (AFPSCharacter* Character = Cast<AFPSCharacter>(MeshComp->GetOwner()))
		{
			if (Character->IsLocallyControlled()) {
				Character->PlayFootStep(Character->GetActorLocation());
			}
		}
	}
}
