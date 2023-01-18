// Fill out your copyright notice in the Description page of Project Settings.


#include "pHUD.h"

UpHUD::UpHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UpHUD::UpdateShield(float Shield)
{
	if(this)
	SP = Shield;
}

void UpHUD::UpdateHealth(float Health)
{
	//TODO: Fix nullptr crash here
	if(this)
	HP = Health;
}
