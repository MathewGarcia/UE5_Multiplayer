// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "Bomb.h"
#include "BombSite.h"
#include "DeadPlayerInfo.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
#include "Engine.h"
#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneComponent.h"
#include "PlayerHUD.h"
#include "FPSPlayerController.h"
#include "PlayerInfoState.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputConfigData.h"
#include "DrawDebugHelpers.h"
#include "FPSGameState.h"
#include "InputMappingContext.h"
#include "Math/Rotator.h"
#include "GrenadeWeapon.h"
#include "ItemSpawnPoint.h"
#include "Landscape.h"
#include "LandscapeComponent.h"
#include "LandscapeLayerInfoObject.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SlateWrapperTypes.h"
#include "FPSMMO/FPSMMOGameModeBase.h"
#include "pHUD.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "ProceduralMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Rendering/StaticMeshVertexBuffer.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundCue.h"

#define ECC_Projectile ECC_GameTraceChannel9
#define SurfaceType_Grass SurfaceType1

// TODO POSSIBLY CHANGE SOME BULLETS TO BE RAYCAST VS SOME TO BE PROJECTILES??? (MAY JUST SCRAP THIS IDEA)
//TODO fix grenades
// TODO add portal that links 2 sides of the map. These portals will be on a timer. ex: loop a 2 min timer to turn on for 15 seconds, then shut back off.
// Sets default values
AFPSCharacter::AFPSCharacter()
{
	bReplicates = true;
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
		// Set size for collision capsule
		GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

		// Create a follow camera
		FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
		FPSCameraComponent->SetupAttachment(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
		FPSCameraComponent->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
		FPSCameraComponent->PostProcessSettings.bOverride_ColorSaturation = true;
		


		// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
		FPSMeshArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSMesh"));
		FPSMeshArms->SetOnlyOwnerSee(true);
		FPSMeshArms->SetupAttachment(FPSCameraComponent);
		FPSMeshArms->bCastDynamicShadow = false;
		FPSMeshArms->CastShadow = false;

		WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
		WeaponMesh->SetupAttachment(FPSCameraComponent);
		WeaponMesh->SetOnlyOwnerSee(true);


		thirdPersonPlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("thirdPersonPlayerMesh"));
		thirdPersonPlayerMesh->SetupAttachment(RootComponent);

		// Make sure the static mesh is visible
		thirdPersonPlayerMesh->SetVisibility(true);
		thirdPersonPlayerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		thirdPersonPlayerMesh->SetOnlyOwnerSee(false);
		CrouchEyeOffset = FVector(0.f);
		CrouchSpeed = 12.f;

	//dismemberment bones with corresponding damage.
		BoneDamageThreshold.Add(FName("head"), FBoneDamageInfo{0.0f,100.f});
		BoneDamageThreshold.Add(FName("r_Leg"), FBoneDamageInfo{ 0.0f,100.f });
		BoneDamageThreshold.Add(FName("l_Leg"), FBoneDamageInfo{ 0.0f,100.f });
		BoneDamageThreshold.Add(FName("r_Arm"), FBoneDamageInfo{ 0.0f,100.f });
		BoneDamageThreshold.Add(FName("l_Arm"), FBoneDamageInfo{ 0.0f,100.f });

	//set bcancrouch to true
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

		SlideStartTime = 0;

		DefaultFOV = 90;



}

void AFPSCharacter::Move(const FInputActionValue& Value)
{
	if (bMenuOpen) return;	
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);

		// Forward/Backward direction
		if (MoveValue.Y != 0.f)
		{
			// Get forward vector
			const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);

			AddMovementInput(Direction, MoveValue.Y);
		}

		// Right/Left direction
		if (MoveValue.X != 0.f)
		{
			// Get right vector
			const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);

			AddMovementInput(Direction, MoveValue.X);
		}
	}
}

void AFPSCharacter::Look(const FInputActionValue& Value)
{
	if (bMenuOpen) return;
	if (Controller != nullptr)
	{
		const FVector2D LookValue = Value.Get<FVector2D>();

		if (LookValue.X != 0.f)
		{
			AddControllerYawInput(LookValue.X*MouseSens);
		}

		if (LookValue.Y != 0.f)
		{
			AddControllerPitchInput(LookValue.Y*MouseSens);
		}
	}
}

void AFPSCharacter::SetCombatStatus(AController* EventInstigator)
{
	if (EventInstigator) {
		APlayerInfoState* instigatorPlayerState = Cast<APlayerInfoState>(EventInstigator->PlayerState);

		if (instigatorPlayerState)
		{
			instigatorPlayerState->SetPlayer(Cast<AFPSCharacter>(instigatorPlayerState->GetPawn()));
			if (instigatorPlayerState->GetInCombat() == true) {
				//clear previous timer
				if (GetWorld()->GetTimerManager().TimerExists(instigatorPlayerState->RechargeHandle)) {
					GetWorld()->GetTimerManager().ClearTimer(instigatorPlayerState->RechargeHandle);
				}
				
					//set new timer
					GetWorld()->GetTimerManager().SetTimer(instigatorPlayerState->RechargeHandle, instigatorPlayerState, &APlayerInfoState::InCombatFalse, instigatorPlayerState->RechargeRate, false);


			}

			if (instigatorPlayerState->GetInCombat() != true)
				instigatorPlayerState->OnRep_InCombat();

			
		}
	}

	APlayerInfoState* PS = Cast<APlayerInfoState>(GetPlayerState());

	if (PS)
	{

		if (PS->GetInCombat() == true)
		{
			if (GetWorld()->GetTimerManager().TimerExists(PS->RechargeHandle)) {
				GetWorld()->GetTimerManager().ClearTimer(PS->RechargeHandle);
			}

				GetWorld()->GetTimerManager().SetTimer(PS->RechargeHandle, PS, &APlayerInfoState::InCombatFalse, PS->RechargeRate, false);
			
		}

		if(PS->GetInCombat() != true)
		PS->OnRep_InCombat();

		
	}
}

void AFPSCharacter::WeaponSwap(const FInputActionValue& Value)
{
	float Val = Value.Get<float>();
	//call Swap function
	SwitchWeapon(Val);
	
}

void AFPSCharacter::SwitchWeapon_Implementation(float Direction)
{

	int currentIndex = EquippedWeapons.IndexOfByKey(CurrentWeapon);
	PreviousWeapon = EquippedWeapons[currentIndex];

		if (Direction > 0)
		{
			//this means we're going up
			if (EquippedWeapons.Num() > 1)
			{
				int next = (currentIndex+1) % EquippedWeapons.Num();
				if (CurrentWeapon != EquippedWeapons[next]) {
					UseWeapon(EquippedWeapons[next]);
				}
			}
		}
		else
		{
			if (EquippedWeapons.Num() > 1) {
				int last = (currentIndex - 1 + EquippedWeapons.Num()) % EquippedWeapons.Num();
				if (CurrentWeapon != EquippedWeapons[last]) {
					UseWeapon(EquippedWeapons[last]);
				}
			}


		}
	
}

void AFPSCharacter::OnRep_Sprint() const
{
	if(bSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = 1200.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
}

void AFPSCharacter::ServerSetClimbing_Implementation(ClimbingState ClimbingState)
{
	SetClimbing(ClimbingState);
}

bool AFPSCharacter::ServerSetClimbing_Validate(ClimbingState ClimbingState)
{
	return true;
}


void AFPSCharacter::SetClimbing(ClimbingState ClimbingState)
{
	if (ClimbingState == ClimbingState::NotClimbing) {
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	CurrentClimbingState = ClimbingState;
}


void AFPSCharacter::PerformClimb(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Climbing: %d"), ClimbCounter);

    if (ClimbCounter >= 3)  // Max 3 climbs
    {
		UE_LOG(LogTemp, Warning, TEXT("CLIMBING IS AT 3"));	
		SetClimbing(ClimbingState::NotClimbing);
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		LaunchCharacter(FVector(0, 0, -150), true, true);
        return;
    }

	float CurrentTime = GetWorld()->GetTimeSeconds();


	switch (CurrentClimbingState)
    {
    case ClimbingState::NotClimbing:
        // Initialization logic or other stuff here, if needed
        break;

    case ClimbingState::InitialClimb:
		UE_LOG(LogTemp, Warning, TEXT("Entered InitialClimb"));
		LaunchCharacter(FVector(0, 0, 400), true, true);
		SetClimbing(ClimbingState::FallingBack);
		TimeEnteredFallingBack = CurrentTime;

		break;

	case ClimbingState::FallingBack:

		GetCharacterMovement()->SetMovementMode(MOVE_Falling);

		if(CurrentTime - TimeEnteredFallingBack >= 0.4f)
		{
			SetClimbing(ClimbingState::Climbing);
		}
        break;

    case ClimbingState::Climbing:
		UE_LOG(LogTemp, Warning, TEXT("Entered Climbing"));

		if (IsButtonPressed("IA_Jump") && IsLookingAtWall()) {
			ClimbCounter++;
			LaunchCharacter(FVector(0, 0, 550), true, true);
			SetClimbing(ClimbingState::FallingBack);
			TimeEnteredFallingBack = CurrentTime;
		}
		else
		{
			SetClimbing(ClimbingState::NotClimbing);
		}
		break;
    }

}



void AFPSCharacter::ServerPerformClimb_Implementation(float DeltaTime)
{
	PerformClimb(DeltaTime);
}

bool AFPSCharacter::ServerPerformClimb_Validate(float DeltaTime)
{
	return true;
}


void AFPSCharacter::ApplyRecoil()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->CurrentRecoil = FRotator(FMath::FRandRange(0, CurrentWeapon->MaxVerticalRecoil),
			FMath::FRandRange(-CurrentWeapon->MaxHorizontalRecoil, CurrentWeapon->MaxHorizontalRecoil), 0);

		TargetControllerRecoil = FMath::FRandRange(-CurrentWeapon->MaxVerticalRecoil, 0);
	}
}



void AFPSCharacter::ServerStartSprint_Implementation()
{
	if (!bSprint) {
		SetSprint(bSprint, true);
	}
	else
	{
		SetSprint(bSprint, false);
	}
}

bool AFPSCharacter::ServerStartSprint_Validate()
{
	return true;
}

AWeapon* AFPSCharacter::FindWeaponInOverlap(AWeapon*WeaponToEquip)
{
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	float radius = 100.f;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), radius , ObjectTypes, AWeapon::StaticClass(), TArray<AActor*>(), OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		AWeapon* Weapon = Cast<AWeapon>(Actor);

		if (Weapon && Weapon == WeaponToEquip)
		{
			return Weapon;
		}
	}

	// Return nullptr if no matching weapon is found.
	return nullptr;
}

void AFPSCharacter::SetSprint(bool &bSprintVal, bool Val)
{
	if (HasAuthority()) {
		bSprintVal = Val;
		OnRep_Sprint();
	}

}

bool AFPSCharacter::GetSprint()
{
	return bSprint;
}

void AFPSCharacter::StartSprint()
{
		ServerStartSprint();
}

void AFPSCharacter::StartCrouch()
{

	if (!bIsCrouched) {
			 if (CanSlide())
			 {
			 	if(HasAuthority())
				{
					SetSliding(true);
				}
				else
				{
					ServerSlide();
				}
			 }
			 else {

				 Crouch();
			 }
		 }
		 else
		 {
			 UnCrouch();
		 }
	 
}

void AFPSCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if(HalfHeightAdjust == 0.f)
	{
		return;
	}
	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight + HalfHeightAdjust;
	FPSCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);

}

void AFPSCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0.f)
	{
		return;
	}
	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight - HalfHeightAdjust;
	FPSCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
}

void AFPSCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	
	Super::CalcCamera(DeltaTime, OutResult);

	if (FPSCameraComponent)
	{
		FPSCameraComponent->GetCameraView(DeltaTime, OutResult);
		OutResult.Location += CrouchEyeOffset;
	}

}

bool AFPSCharacter::GetSliding()
{
	return bIsSliding;
}

void AFPSCharacter::OnRep_Sliding()
{
	SetSliding(bIsSliding);
}

bool AFPSCharacter::CanSlide()
{
	if (GetCharacterMovement()->IsMovingOnGround() && GetCharacterMovement()->Velocity.Size() >= 1000.0f && bIsSliding == false)
	{
		return true;
	}
	return false;
}

void AFPSCharacter::SetSliding(bool bInSliding)
{
	if(bIsSliding == bInSliding)
	{
		return;
	}


	bIsSliding = bInSliding;

	if(HasAuthority() && bIsSliding)
	{
		InitialSlideVelocity = GetCharacterMovement()->Velocity;
	}

	ApplySlide();
}


void AFPSCharacter::ApplySlide()
{
	SlideStartTime = GetWorld()->GetTimeSeconds();

	if (bIsSliding)
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(20.f); // this is the crouched half height
		GetCharacterMovement()->Crouch(false);
	}
	else
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(88.f); // this is the standing half height
		GetCharacterMovement()->UnCrouch(false);

	}

} 
void AFPSCharacter::AdjustCameraForSlide(bool bInSliding, float DeltaTime)
{

	float TargetCameraZ = bInSliding ? 20.f : 64.f;
	FVector CurrentCameraLocation = FPSCameraComponent->GetRelativeLocation();
	FVector TargetCameraLocation = FVector(0.f, 0.f, TargetCameraZ);
	FVector NewCameraLocation = FMath::VInterpTo(CurrentCameraLocation, TargetCameraLocation, DeltaTime, CameraInterpSpeed);
	FPSCameraComponent->SetRelativeLocation(NewCameraLocation);

}

void AFPSCharacter::ServerEndSlide_Implementation()
{
		EndSlide();
}

bool AFPSCharacter::ServerEndSlide_Validate()
{
		return true;
	
}

void AFPSCharacter::EndSlide()
{

		SetSliding(false);
		InitialSlideVelocity = FVector::ZeroVector;

}

void AFPSCharacter::ServerSlide_Implementation()
{
	SetSliding(true);
}

bool AFPSCharacter::ServerSlide_Validate()
{
	return true;
}

void AFPSCharacter::PerformSlide(float DeltaTime)
{
	GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity.GetSafeNormal() * 1500.0f;

	// Get ground slope
	FHitResult Hit;
	FVector StartTrace = GetActorLocation();
	FVector EndTrace = StartTrace - FVector(0.f, 0.f, 200.f); 
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, QueryParams);

	GroundSlope = FVector::DotProduct(Hit.ImpactNormal, InitialSlideVelocity.GetSafeNormal());

	// Decrease speed due to friction
	float Friction = 200.0f; 
	InitialSlideVelocity -= Friction * DeltaTime * InitialSlideVelocity.GetSafeNormal();
	// Calculate the slide direction
	FVector Down = -GetActorUpVector();
	FVector Right = FVector::CrossProduct(Down, Hit.ImpactNormal);
	FVector SlideDirection;
	if (FMath::Abs(GroundSlope) < 0.01f)  // Adjust the threshold as necessary
	{
		SlideDirection = InitialSlideVelocity.GetSafeNormal();
	}
	else
	{
		SlideDirection = FVector::CrossProduct(Hit.ImpactNormal, Right).GetSafeNormal();
	}


	float CurrentSlideSpeed = GroundSlope * InitialSlideVelocity.Size() * 2;
	float NewSlideSpeed = FMath::Max(MinimumSlideSpeed, CurrentSlideSpeed);
	GetCharacterMovement()->Velocity = SlideDirection * NewSlideSpeed;
}


void AFPSCharacter::ServerSetADS_Implementation(bool NewADS)
{
		bIsADS = NewADS;
		ADS();
	
}

bool AFPSCharacter::ServerSetADS_Validate(bool NewADS)
{
	return true;
}


void AFPSCharacter::UpdateCameraFOV()
{
	if(bIsADS)
	{
		FPSCameraComponent->SetFieldOfView(DefaultFOV / 2);
	}
	else
	{
		FPSCameraComponent->SetFieldOfView(DefaultFOV);

	}
}

void AFPSCharacter::ADS()
{
	if(bIsADS)
	{
		GetCharacterMovement()->MaxWalkSpeed = 200.f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
	if(IsLocallyControlled())
	UpdateCameraFOV();
}

void AFPSCharacter::SetADS(bool Val)
{
	bIsADS = Val;
}

void AFPSCharacter::OnRep_IsADS()
{
	ADS();
}

bool AFPSCharacter::getADS()
{
	return bIsADS;
}

void AFPSCharacter::UpdateWeaponTransform(float DeltaTime)
{
	if (bMenuOpen) return;
	if (IsLocallyControlled()) {
		FTransform CurrentTransform = FPSMeshArms->GetRelativeTransform();
		FVector CurrentLocation = CurrentTransform.GetLocation();
		FQuat CurrentRotation = CurrentTransform.GetRotation();
		if (bIsADS)
		{
			// We are in ADS mode, interpolate towards ADS transform
			FVector TargetLocation = ADSWeaponTransform.GetLocation();
			FQuat TargetRotation = ADSWeaponTransform.GetRotation();

			FVector NewLocation = FMath::Lerp(CurrentLocation, TargetLocation, ADSInterpSpeed * DeltaTime);
			FQuat NewRotation = FQuat::Slerp(CurrentRotation, TargetRotation, ADSInterpSpeed * DeltaTime);



			FPSMeshArms->SetRelativeLocation(NewLocation);
			FPSMeshArms->SetRelativeRotation(NewRotation);
			WeaponMesh->AttachToComponent(FPSMeshArms, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "ADS_Socket");


		}
		else
		{
			// We are not in ADS mode, interpolate towards default transform
			FVector TargetLocation = DefaultWeaponTransform.GetLocation();
			FQuat TargetRotation = DefaultWeaponTransform.GetRotation();

			FVector NewLocation = FMath::Lerp(CurrentLocation, TargetLocation, ADSInterpSpeed * DeltaTime);
			FQuat NewRotation = FQuat::Slerp(CurrentRotation, TargetRotation, ADSInterpSpeed * DeltaTime);

			FPSMeshArms->SetRelativeLocation(NewLocation);
			FPSMeshArms->SetRelativeRotation(NewRotation);
			WeaponMesh->AttachToComponent(FPSMeshArms, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Weapon");

		}
	}
}

bool AFPSCharacter::CanFire()
{

	if (CurrentWeapon && CurrentWeapon->AmmoInClip != 0 && bCanFire && !CurrentWeapon->bIsReloading && !GetPlayerHUD()->GetMarketOpen() && !bMenuOpen)
	{
		return true;
	}

	return false;
	
}

void AFPSCharacter::StartReload(const FInputActionValue& InputActionValue)
{
	if(CurrentWeapon)
	{
		CurrentWeapon->ServerStartReload();
	}
}

void AFPSCharacter::ServerDropWeapon_Implementation(AWeapon* DroppedWeapon)
{
	DropWeapon(DroppedWeapon);
}

bool AFPSCharacter::ServerDropWeapon_Validate(AWeapon* DroppedWeapon)
{
	return true;
}

void AFPSCharacter::DropWeapon(AWeapon* DroppedWeapon)
{
		FVector DropDirection = GetActorForwardVector();

		FVector DropLocation = GetActorLocation() + DropDirection * 10.f; 

		FVector TraceEnd = DropLocation - FVector(0, 0, 500.0f); // 500 units down.

		FCollisionQueryParams QueryParams;

		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		FHitResult Hit;

		if (GetWorld()->LineTraceSingleByChannel(Hit, DropLocation, TraceEnd, ECC_Visibility))
		{
			DroppedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			DroppedWeapon->bIsAttached = false;
			DroppedWeapon->SetActorLocation(Hit.Location);
			DroppedWeapon->ServerSetPickUp(false);
		}
}

void AFPSCharacter::SetSensitivity(float NewMouseSens)
{
	MouseSens = NewMouseSens;
}

void AFPSCharacter::SetCanPlant(bool bCanPlant)
{
	CanPlant = bCanPlant;
}

void AFPSCharacter::OpenMenu(const FInputActionValue& Value)
{

	if (FPSPC) {
		if (!bMenuOpen) {

			FPSPC->SetMenuVisibility(ESlateVisibility::Visible);
			FInputModeGameAndUI PausedMode;
			PausedMode.SetWidgetToFocus(GetPlayerHUD()->HUDWidget->TakeWidget());
			FPSPC->SetInputMode(PausedMode);
			FPSPC->SetShowMouseCursor(true);
			bMenuOpen = true;
		}
		else
		{
			FPSPC->SetMenuVisibility(ESlateVisibility::Collapsed);
			FPSPC->SetInputMode(FInputModeGameOnly());
			FPSPC->SetShowMouseCursor(false);
			bMenuOpen = false;

		}
	}
}

bool AFPSCharacter::GetCanPlant()
{
	return CanPlant;
}

FString AFPSCharacter::GetKey(const FString& ActionName)
{
	if (InputMapping) {

		const TArray<FEnhancedActionKeyMapping>& Mappings = InputMapping->GetMappings();

		for (const FEnhancedActionKeyMapping& Mapping : Mappings)
		{
			if(Mapping.Action->GetFName() == FName(*ActionName))
			{
				return Mapping.Key.ToString();
			}
		}
	}

	return FString();
}

void AFPSCharacter::OpenTaccom(const FInputActionValue& InputActionValue)
{
	if(!InputActionValue.Get<bool>())
	{
		//this is our taccom post process
		FPSCameraComponent->PostProcessSettings.WeightedBlendables.Array[0].Weight = 0.0f;
		if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(GetController())) {
			PC->UpdatePostProcess(FMath::Clamp(HP / MaxHP, 0, 1));
		}

	}
	else
	{
		FPSCameraComponent->PostProcessSettings.WeightedBlendables.Array[0].Weight = 1.0f;
		FPSCameraComponent->PostProcessSettings.ColorSaturation = FVector4(1.f, 1.f, 1.f, 1.f);

	}
		GetPlayerHUD()->OpenTaccom(InputActionValue.Get<bool>());
}

void AFPSCharacter::ServerBombInteraction_Implementation()
{
	if(BombInteractionType == EBombInteractionType::Planting)
	{
		ABomb* Bomb = GetWorld()->SpawnActor<ABomb>(bomb);
		if(Bomb)
		{
			Bomb->PlaceAtLocation(GetActorLocation());
			Bomb->SetTeam(Cast<APlayerInfoState>(GetPlayerState())->TeamId);
			if(GS)
			{
				GS->bIsBombPlanted = true;
				GS->OnBombPlanted(Bomb);

				for (ABombSite* BombSite : GS->BombSites)
				{
					if (BombSite->IsOverlappingActor(this))
					{
						BombSite->SetPlantedBomb(Bomb);
						UnCrouch();
						StopPlanting();
						BombSite->bBoxComponentEnabled = false;
						return;
					}
					
				}
			}
		}
	}
	else if (BombInteractionType == EBombInteractionType::Defusing)
	{
		//defuse the bomb
		if (GS) {
			if (GetWorld()->GetTimerManager().IsTimerActive(GS->BombTimerHandle))
			{
				GetWorld()->GetTimerManager().ClearTimer(GS->BombTimerHandle);

				for (ABombSite* BombSite : GS->BombSites)
				{
					if (BombSite->GetPlantedBomb()->IsOverlappingActor(this))
					{
						BombSite->GetPlantedBomb()->Destroy();
						GS->bIsBombPlanted = false;
						BombSite->SetPlantedBomb(nullptr);
						BombSite->bBoxComponentEnabled = true;
						UnCrouch();
						StopPlanting();
						break;
					}
				}
			}
		}
	}

}

bool AFPSCharacter::ServerBombInteraction_Validate()
{
	return true;
}

void AFPSCharacter::StopPlanting()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling stop planting"));
	GetController()->SetIgnoreMoveInput(false);
	bIsMovementIgnored = false;
	ServerSetBombInteraction(EBombInteractionType::None);
}

void AFPSCharacter::SetDeathEXP(int32 NewDeathEXP)
{
	DeathEXP = NewDeathEXP;
}

void AFPSCharacter::SetDeathGold(int32 NewDeathGold)
{
	DeathGold = NewDeathGold;
}

int32 AFPSCharacter::GetDeathEXP()
{
	return DeathEXP;
}

int32 AFPSCharacter::GetDeathGold()
{
	return DeathGold;
}

void AFPSCharacter::PrepareForInitialization()
{
	OnPlayerReady.Broadcast(this);
}

bool AFPSCharacter::IsLookingAtWall()
{
	// Existing code: Setting up the line trace to check if we're facing a wall
	FVector Start, ForwardVector, End;
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	Start = GetActorLocation() + FVector(0, 0, 50);
	ForwardVector = GetActorForwardVector();
	End = ((ForwardVector * 150.f) + Start);

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, true);

	FVector HitNormal = HitResult.ImpactNormal;
	FVector UpVector = FVector::UpVector;
	float wallAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitNormal, UpVector)));

	// New code: Checking if the camera's direction is nearly aligned with the wall
	FVector CameraForwardVector = FPSCameraComponent->GetForwardVector(); // Assume FPSCameraComponent is your camera component
	float dotProduct = FVector::DotProduct(CameraForwardVector, -HitNormal);  // Negate HitNormal to check for the opposite direction

	// Combined condition: Check the wall's angle and the camera's direction
	if (HitResult.bBlockingHit && wallAngle >= 85.0f && wallAngle <= 95.0f && dotProduct > 0.90f && dotProduct <= 1.0f)
	{
		return true;
	}

	return false;
}


void AFPSCharacter::ServerSpawnWeapon_Implementation(TSubclassOf<AWeapon> WeaponClass)
{
	FActorSpawnParameters SpawnParameters;

	SpawnParameters.Owner = this;

	SpawnParameters.Instigator = GetInstigator();

	FVector Location = GetActorLocation();
	FRotator Rotation = GetActorRotation();

	AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, Location, Rotation, SpawnParameters);
	if(NewWeapon)
	EquipWeapon(NewWeapon);
}

bool AFPSCharacter::ServerSpawnWeapon_Validate(TSubclassOf<AWeapon> WeaponClass)
{
	return WeaponClass != nullptr;
}


void AFPSCharacter::ServerSetBombInteraction_Implementation(EBombInteractionType InteractionType)
{
	BombInteractionType = InteractionType;

	UE_LOG(LogTemp, Warning, TEXT("InteractionType: %s"), *UEnum::GetValueAsString(BombInteractionType));

	if (GS && FPSPC) {
		if (InteractionType == EBombInteractionType::None && GetWorld()->GetTimerManager().IsTimerActive(BombInteractionTimerHandle) && GS->bIsBombPlanted)
		{
			// Interaction was stopped before the timer ran out. Clear the timer.
			GetWorld()->GetTimerManager().ClearTimer(BombInteractionTimerHandle);
			GetController()->SetIgnoreMoveInput(false);

		}
			// Started interacting. Set the timer.
		else if (InteractionType == EBombInteractionType::Planting) {
				GetWorld()->GetTimerManager().SetTimer(BombInteractionTimerHandle, this, &AFPSCharacter::ServerBombInteraction, BombInteractionTime, false);

		}
		else if(InteractionType == EBombInteractionType::Defusing)
			{
			for (ABombSite* BombSite : GS->BombSites)
			{
				if (BombSite->GetPlantedBomb() && BombSite->GetPlantedBomb()->IsOverlappingActor(this))
				{
					BombSite->GetPlantedBomb()->MulticastDefusing();
					GetWorld()->GetTimerManager().SetTimer(BombInteractionTimerHandle, this, &AFPSCharacter::ServerBombInteraction, 7.0f, false);
					break;
				}
				}
			}
		
	}
}

bool AFPSCharacter::ServerSetBombInteraction_Validate(EBombInteractionType InteractionType)
{
	return true;
}

void AFPSCharacter::ServerPerformSlide_Implementation(float DeltaTime)
{

	ServerTick(DeltaTime);

}

bool AFPSCharacter::ServerPerformSlide_Validate(float DeltaTime)
{
	return true;
}

void AFPSCharacter::CopySkeletalMeshToPrecedual(USkeletalMeshComponent* SkeletalMeshComponent, int32 LODIndex, UProceduralMeshComponent* ProcMeshComponent)
{
	FSkeletalMeshRenderData* SkeletalMeshRenderData = SkeletalMeshComponent->GetSkeletalMeshRenderData();
	const FSkeletalMeshLODRenderData& DataArray = SkeletalMeshRenderData->LODRenderData[LODIndex];
	FSkinWeightVertexBuffer& SkinWeights = *SkeletalMeshComponent->GetSkinWeightBuffer(LODIndex);

	TArray<FVector> VerticesArray;
	TArray<FVector> Normals;
	TArray<FVector2D> UV;
	TArray<int32> Tris;
	TArray<FColor> Colors;
	TArray<FProcMeshTangent> Tangents;

	int32 NumSourceVertices = DataArray.RenderSections[0].NumVertices;

	for (int i = 0; i < NumSourceVertices; i++)
	{
		FVector SkinnedVectorPos = FVector(SkeletalMeshComponent->GetSkinnedVertexPosition(SkeletalMeshComponent,i,DataArray,SkinWeights));
		VerticesArray.Add(SkinnedVectorPos);

		FVector ZTangentStatic = FVector(DataArray.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(i));
		FVector XTangentStatic = FVector(DataArray.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentX(i));

		Normals.Add(ZTangentStatic);

		Tangents.Add(FProcMeshTangent(XTangentStatic, false));
		FVector2D uvs = FVector2D(DataArray.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(i, 0));
		UV.Add(uvs);
		Colors.Add(FColor(0, 0, 0, 255));
	}
	FMultiSizeIndexContainerData indicesData;
	DataArray.MultiSizeIndexContainer.GetIndexBuffer(indicesData.Indices);

	for (int32 i = 0; i < indicesData.Indices.Num(); i++)
	{
		uint32 a = 0;
		a = indicesData.Indices[i];
		Tris.Add(a);
	}



	ProcMeshComponent->CreateMeshSection(0, VerticesArray, Tris, Normals, UV, Colors, Tangents, true);

}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	FPSCameraComponent->PostProcessSettings.WeightedBlendables.Array[0].Weight = 0.0f;

	if (HasAuthority())
	{
		ensure(GetOwner() != nullptr && "Character should have an owner on the server.");
	}
	bCanFire = true;


		APlayerController* PC = GetWorld()->GetFirstPlayerController();

	if (PC)
	{
		FPSPC = Cast<AFPSPlayerController>(PC);
	}

	if (FPSPC && FPSPC->IsLocalPlayerController())
	{
		GetPlayerHUD()->UpdateText(FText::FromString(TEXT("")));
	}

	if (HasAuthority()) {
		APlayerInfoState* PIS = Cast<APlayerInfoState>(GetPlayerState());
		//if we arent connected, set the conneciton state to connected
		if (PIS && PIS->GetConnectionState() != EConnection::Connected) {
			PIS->SetConnectionState(EConnection::Connected);
		}

		if(PIS)
		{
			MaxShield = PIS->MaxShield;
			SetShield(MaxShield);
		}
		SetHealth(MaxHP);


	}

	DefaultWeaponTransform = FPSMeshArms->GetRelativeTransform();
	NormalWeaponRotation = WeaponMesh->GetRelativeRotation(); 

	GS = GetWorld()->GetGameState<AFPSGameState>();

	
}

void AFPSCharacter::SetHealth(float hp)
{
	if(HasAuthority())
	{
		HP = hp;
		if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(GetController()))
		{
				PC->UpdatePostProcess(FMath::Clamp(HP / MaxHP,0,1));
			
		}
	}
	
}

void AFPSCharacter::SetShield(float shield)
{
	if (HasAuthority()) {
		Shield = FMath::Clamp(shield, 0.0f, MaxShield);
	}
}

void AFPSCharacter::StartTeleport(const FInputActionValue& InputActionValue)
{
	if (GetVelocity().Size() == 0)
	{
		float TotalDuration = GetWorld()->GetTimeSeconds();

		UE_LOG(LogTemp, Warning, TEXT("Starting Teleportation"));
		if(FPSPC)
		{
			FPSPC->ClientShowRecallBar();
		}

		GetWorld()->GetTimerManager().SetTimer(TeleportTimerHandle, [this]()
		{
			Teleport();
		}, 1.f, false);

		GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, [this,TotalDuration]()
		{
			if(GetWorld()->GetTimerManager().IsTimerActive(TeleportTimerHandle))
			{
				if(FPSPC)
				{
					float CurrentWorldTime = GetWorld()->GetTimeSeconds();
					float CurrentRecallTime = (CurrentWorldTime - TotalDuration) / 1.f;
					FPSPC->ClientUpdateRecallProgressBar(CurrentRecallTime);
				}
			}
		}, 0.05, true);
	}
}

void AFPSCharacter::ServerSetHealth_Implementation(float val)
{
	SetHealth(val);
}

bool AFPSCharacter::ServerSetHealth_Validate(float val)
{
	return true;
}

void AFPSCharacter::ServerSetShield_Implementation(float val)
{
	SetShield(val);
}

bool AFPSCharacter::ServerSetShield_Validate(float val)
{
	return true;
}

void AFPSCharacter::OnRep_Health()
{
	if(HP <= 0)
	{
		KilledBy(LastDamagingPlayer);
	}
}

void AFPSCharacter::OnRep_Shield()
{
}

void AFPSCharacter::SetCanOpenMarket(bool val)
{
	bCanOpenMarket = val;
}

bool AFPSCharacter::CanOpenMarket()
{
	return bCanOpenMarket;
}

void AFPSCharacter::OnRep_PreviousWeapon()
{
	if (EquippedWeapons.Num() > 2) {
		if (CurrentWeapon == EquippedWeapons[1])
		{
			PreviousWeapon = EquippedWeapons[0];
		}
		else
		{
			PreviousWeapon = EquippedWeapons[1];
		}
	}
	else if(!EquippedWeapons.IsEmpty())
	{
		PreviousWeapon = EquippedWeapons[0];
	}
}

void AFPSCharacter::OnRep_CurrentWeapon()
{

	if (PreviousWeapon) {
		// Hide the previous weapon
		PreviousWeapon->SetActorHiddenInGame(true);
	}

	if (CurrentWeapon) {
		AttachWeaponToCharacter(CurrentWeapon);
		UE_LOG(LogTemp, Warning, TEXT("OnRep_CurrentWeapon called"));
	}
}

void AFPSCharacter::SetFPSMesh(AWeapon*Weapon)
{
	if (!Weapon) return;

		Weapon->SetOwner(this);
		Weapon->WeaponMesh->SetOwnerNoSee(true);
		//this is client side, if there is a client weapon mesh use that one, this is for big weapons that cause clipping. Else, we want to use the one we picked up.
		WeaponMesh->SetSkinnedAssetAndUpdate(Weapon->WeaponMesh->GetSkinnedAsset());
		WeaponMesh->AttachToComponent(FPSMeshArms, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Weapon");
}

void AFPSCharacter::ServerSpawnProjectile_Implementation(FVector SpawnLocation, FRotator SpawnRotation)
{
	if (HasAuthority()) {
		FActorSpawnParameters spawnParameters;
		spawnParameters.Instigator = this;
		spawnParameters.Owner = this;

		FVector Offset = SpawnRotation.Vector() * 25;
		SpawnLocation += Offset;
		SpawnRotation.Normalize();

		if (CurrentWeapon && CurrentWeapon->ProjectileToSpawn) {
			if (AProjectile* bullet = GetWorld()->SpawnActor<AProjectile>(CurrentWeapon->ProjectileToSpawn, SpawnLocation, SpawnRotation, spawnParameters))
			{
				bullet->SetFiredWeapon(CurrentWeapon);
				Firing();

				if (CurrentWeapon->FiringSound) {
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeapon->FiringSound, GetActorLocation(), GetActorRotation());
				}
				MoveIgnoreActorAdd(bullet);

				if (CurrentWeapon->bIsShotgun)
				{
					bullet->bIsShotgunPellet = true;

				}
				UPrimitiveComponent* BulletPrimitive = Cast<UPrimitiveComponent>(bullet->GetRootComponent());
				if (BulletPrimitive)
				{
					// Ignore the character
					BulletPrimitive->MoveIgnoreActors.Add(this);
					BulletPrimitive->MoveIgnoreActors.Add(CurrentWeapon);
				}

				bullet->GetCollisionComponent()->IgnoreActorWhenMoving(this, true);


				FVector LaunchDirection = SpawnRotation.Vector();
				LaunchDirection *= CurrentWeapon->WeaponDistance;
				bullet->FireInDirection(LaunchDirection);
			}
			if (bIsADS)
				ApplyRecoil();
		}
	}
}

void AFPSCharacter::Firing_Implementation()
{
	if (CurrentWeapon) {
		if (CurrentWeapon->FiringSound) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeapon->FiringSound, GetActorLocation(), GetActorRotation());
		}
	}
}

void AFPSCharacter::SpawnGrenade_Implementation(FVector SpawnLocation, FRotator SpawnRotation)
{
	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = this;
	spawnParameters.Owner = this;

	FVector Offset = SpawnRotation.Vector() * 25;
	SpawnLocation += Offset;
	SpawnRotation.Normalize();
	if (GrenadeWeapon) {
		if (AProjectile* Grenade = GetWorld()->SpawnActor<AProjectile>(GrenadeProjectile, SpawnLocation, SpawnRotation, spawnParameters)) {
			FVector LaunchDirection = SpawnRotation.Vector();
			LaunchDirection *= 20;
			Grenade->FireInDirection(LaunchDirection);
			UE_LOG(LogTemp, Warning, TEXT("Spawned Grenade"));
		}
	}
}



FTransform AFPSCharacter::GetFiringPosition()
{
		// Calculate SpawnLocation and SpawnRotation as you were doing previously
	FVector SpawnLocation;
		FRotator SpawnRotation;

		GetActorEyesViewPoint(SpawnLocation, SpawnRotation);

		return FTransform(SpawnRotation, SpawnLocation);
}

void AFPSCharacter::DestroyCharacter()
{
	if(HasAuthority())
	Destroy();
}


void AFPSCharacter::KilledBy(AController* EventInstigator)
{
	if (EventInstigator) {
		APlayerInfoState* instigatorPlayerState = Cast<APlayerInfoState>(EventInstigator->PlayerState);
		if (instigatorPlayerState)
		{
			instigatorPlayerState->UpdateEXP(CurrentWeapon->GetWeaponEXP()+DeathEXP);
			instigatorPlayerState->UpdateGold(CurrentWeapon->GetWeaponGold()+DeathGold);
		}
	}

	APlayerInfoState* PS = Cast<APlayerInfoState>(GetPlayerState());

	if (PS)
	{
		PS->UpdateDeath();
	}
	if (LastDamagingPlayer)
	{
		APlayerInfoState* EnemyPS = Cast<APlayerInfoState>(LastDamagingPlayer->GetPlayerState<APlayerInfoState>());
		if (EnemyPS)
		{
			EnemyPS->UpdateKills();
		}
	}

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (MovementComponent)
	{
		MovementComponent->StopMovementImmediately();
	}



	UE_LOG(LogTemp, Warning, TEXT("HitPoints: %d"), HitResults.Num());


	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	thirdPersonPlayerMesh->SetCollisionProfileName(TEXT("Ragdoll"));
	thirdPersonPlayerMesh->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
	thirdPersonPlayerMesh->SetAllBodiesSimulatePhysics(true);
	thirdPersonPlayerMesh->SetSimulatePhysics(true);
	thirdPersonPlayerMesh->WakeAllRigidBodies();
	thirdPersonPlayerMesh->bBlendPhysics = true;


	FPSMeshArms->SetHiddenInGame(true);
	WeaponMesh->SetHiddenInGame(true);


	if (GetController())
	{
		AFPSMMOGameModeBase* GM = Cast<AFPSMMOGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GM && GS)
		{
			FDeadPlayerInfo DeadPlayer(GetController(), GM->GetRespawnTime());
			GS->DeadPlayers.Add(DeadPlayer);
		}
	}

	GetWorldTimerManager().SetTimer(DestructionTimer, this, &AFPSCharacter::DestroyCharacter, TimeBeforeDestroy, false);

}

void AFPSCharacter::Interact()
{
	//if we arent interacting with the bomb site.
	UE_LOG(LogTemp, Warning, TEXT("CanPlant: %s"), CanPlant ? TEXT("TRUE") : TEXT("FALSE"));

	if (IsButtonPressed("IA_Interact")) {
		if (!CanPlant) {

			HandleInteract();

			if (FPSPC && FPSPC->IsPlayerController()) {
				if (IsButtonPressed("IA_Interact")) {
					APlayerHUD* PHUD = GetPlayerHUD();
					if (bCanOpenMarket && PHUD)
					{
						PHUD->GetMarketOpen() ? PHUD->HideMarket() : PHUD->ShowMarket();
						PHUD->UpdateText(FText::FromString(""));
					}
					else
					{
						PHUD->HideMarket();
					}
				}
			}	
		}
	}

		if (!IsButtonPressed("IA_Interact"))
		{
			UnCrouch();
			StopPlanting();
			return;
		}

		if (BombInteractionType != EBombInteractionType::None)
		{
			return;
		}

	

	

	//if we are not planting, can plant and are not collided with a weapon, we start to crouch, we set serverplanting to true, and then if we hold the button long enough, server plant will be called.
	//knowing interact has a released trigger (this function will be called upon release of the trigger) in IA_Interact this function will be called again, if we did not finish planting (bIsPlanting is true)
	//and are still in the trigger, and the timer still exists, we cancel the plant. If the timer does not exist, that means we finished planting. Which in ServerPlant calls bIsPlanting = false;
	if (GS) {
		if (FPSPC) {
			if (CanPlant && BombInteractionType == EBombInteractionType::None && !WeaponCollided && !GS->bIsBombPlanted) {
				if (!bIsMovementIgnored) {
					UE_LOG(LogTemp, Warning, TEXT("Calling Set Ignore Move Input(planting)"));
					GetController()->SetIgnoreMoveInput(true);
					bIsMovementIgnored = true;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Set Ignore Move Input is already true"));

				}
				for(ABombSite*BombSite : GS->BombSites)
				{
					if(BombSite && BombSite->IsOverlappingActor(this) && BombSite->GetTeam() != Cast<APlayerInfoState>(GetPlayerState())->TeamId)
					{
						StartCrouch();
						ServerSetBombInteraction(EBombInteractionType::Planting);
						return;
					}
				}

			}
			// Otherwise, if we are planting, then stop planting and uncrouch.
			else if ( (CanPlant && BombInteractionType == EBombInteractionType::Planting) || (BombInteractionType == EBombInteractionType::Defusing && GS->bIsBombPlanted)) {
				StopPlanting();
				UnCrouch();
			}
			else if (GS->bIsBombPlanted && BombInteractionType == EBombInteractionType::None)
			{
				if (!bIsMovementIgnored) {
					UE_LOG(LogTemp, Warning, TEXT("Calling Set Ignore Move Input(defuse)"));
					GetController()->SetIgnoreMoveInput(true);
					bIsMovementIgnored = true;
				}
				for(ABombSite* BombSite : GS->BombSites)
				{
					if( BombSite->GetPlantedBomb()  && BombSite->GetPlantedBomb()->IsOverlappingActor(this))
					{
						if(BombSite->GetPlantedBomb()->GetTeam() != Cast<APlayerInfoState>(GetPlayerState())->TeamId)
						{
							StartCrouch();
							ServerSetBombInteraction(EBombInteractionType::Defusing);
							break;
						}
					}
				}

			}
		}
	}
}

bool AFPSCharacter::IsButtonPressed(FString Button)
{
	if (FPSPC && FPSPC->PlayerInput)
	{
		return FPSPC->PlayerInput->IsPressed(FKey(*GetKey(Button)));
	}

	return false;
}


void AFPSCharacter::HandleInteract_Implementation()
{
	if (HasAuthority()) {
		if (bInCollision && WeaponCollided)
		{
			bool isWeaponEligible = false;
			//check if the spawn point exists
			if (WeaponCollided->GetSpawnPoint()) {
				//if it does check if the spawn point teamID matches our teamID this will alternate based on if we match teamID or not
				isWeaponEligible = WeaponCollided->GetSpawnPoint()->SpawnedTeam == Cast<APlayerInfoState>(GetPlayerState())->TeamId;

			}
			else {
				//if the spawn point doesnt exist aka the weapon was dropped etc. it no longer matters
				isWeaponEligible = true;
			}
			if (isWeaponEligible) {
				//add the weapon to the equip
				EquipWeapon(WeaponCollided);
			}
		}
	}
	else
	{

		if (bInCollision && WeaponCollided)
		{
			bool isWeaponEligible = false;
			//check if the spawn point exists
			if (WeaponCollided->GetSpawnPoint()) {
				//if it does check if the spawn point teamID matches our teamID
				isWeaponEligible = WeaponCollided->GetSpawnPoint()->SpawnedTeam == Cast<APlayerInfoState>(GetPlayerState())->TeamId;

			}
			else {
				//if the spawn point doesnt exist aka the weapon was dropped etc. it no longer matters
				isWeaponEligible = true;
			}
			if (isWeaponEligible) {
				//add the weapon to the equip
				ServerEquipWeapon(WeaponCollided);
			}
		}
	}
}

void AFPSCharacter::DetachFromControllerPendingDestroy()
{
	AController* ControllerRef = GetController();
	UE_LOG(LogTemp, Warning, TEXT("In DetatchFromControllerPendingDestroy Controller: %s"), *GetNameSafe(ControllerRef));

	Super::DetachFromControllerPendingDestroy();

	//this may be able to be removed

/*	if(ControllerRef)
	{
		AFPSMMOGameModeBase* GM = Cast<AFPSMMOGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GM && GS)
		{
			FDeadPlayerInfo DeadPlayer(ControllerRef, GM->GetRespawnTime()); 
			GS->DeadPlayers.Add(DeadPlayer);
		}
	}*/

}

void AFPSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	UE_LOG(LogTemp, Warning, TEXT("Possessed by: %s"), *NewController->GetName());
	UE_LOG(LogTemp, Warning, TEXT("IsLocallyControlled: %s"), IsLocallyControlled() ? TEXT("true") : TEXT("false"));
}


// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	float CrouchInterpTime = FMath::Min(1.f, CrouchSpeed * DeltaTime);
	CrouchEyeOffset = (1.f - CrouchInterpTime) * CrouchEyeOffset;


	if(CurrentClimbingState != ClimbingState::NotClimbing)
	{

		if(HasAuthority())
		{
			PerformClimb(DeltaTime);
		}
		else
		{
			ServerPerformClimb(DeltaTime);
		}
		return;
	}
	
	if (bIsSliding)
	{
		PerformSlide(DeltaTime);
	}
	if(HasAuthority())
	{
		ServerPerformSlide(DeltaTime);
	}
		AdjustCameraForSlide(bIsSliding, DeltaTime);

		UpdateWeaponTransform(DeltaTime);

		if (CurrentWeapon && Controller)
		{
			if (bCanFire)
			{
				// Recover the weapon position when not firing
				FRotator CurrentRotation = WeaponMesh->GetRelativeRotation();
				FRotator InterpolatedRotation = FMath::RInterpTo(CurrentRotation, OriginalSocketRotation, DeltaTime, CurrentWeapon->RecoverySpeed);
				WeaponMesh->SetRelativeRotation(InterpolatedRotation);
			}
			else
			{
				// Apply the current recoil to the weapon
				CurrentWeapon->CurrentRecoil = FMath::RInterpTo(CurrentWeapon->CurrentRecoil, FRotator::ZeroRotator, DeltaTime, CurrentWeapon->RecoilInterpSpeed);

				FRotator NewWeaponRotation = WeaponMesh->GetRelativeRotation() + CurrentWeapon->CurrentRecoil;

				// Clamp the pitch so the weapon doesn't flip upside down
				NewWeaponRotation.Pitch = FMath::Clamp(NewWeaponRotation.Pitch, -CurrentWeapon->MaxVerticalRecoil, CurrentWeapon->MaxVerticalRecoil);
				NewWeaponRotation.Yaw = FMath::Clamp(NewWeaponRotation.Yaw, -CurrentWeapon->MaxHorizontalRecoil, CurrentWeapon->MaxHorizontalRecoil);

				// Apply new rotation to the weapon
				WeaponMesh->SetRelativeRotation(NewWeaponRotation);

				if (AController* PC = GetController())
				{
					FRotator CurrentControlRotation = PC->GetControlRotation();
					CurrentControlRotation.Pitch += CurrentWeapon->CurrentRecoil.Pitch * CurrentWeapon->ControllerRecoilFactor;
					CurrentControlRotation.Yaw += CurrentWeapon->CurrentRecoil.Yaw * CurrentWeapon->ControllerRecoilFactor;
					PC->SetControlRotation(CurrentControlRotation);
				}
			}
		}
		if (GetVelocity().Size() > 0) {
			if (FPSPC)
			{
				FPSPC->ClientHideRecallBar();
				FPSPC->ClientUpdateRecallProgressBar(0);

			}
			if (GetWorld()->GetTimerManager().IsTimerActive(TeleportTimerHandle))
			{
				GetWorld()->GetTimerManager().ClearTimer(TeleportTimerHandle);
				UE_LOG(LogTemp, Warning, TEXT("Stopped Timer"));
			}
			if (GetWorld()->GetTimerManager().IsTimerActive(RecallTimerHandle))
			{
				GetWorld()->GetTimerManager().ClearTimer(RecallTimerHandle);
				UE_LOG(LogTemp, Warning, TEXT("Stopped Recall Timer"));
			}
		}
}

void AFPSCharacter::ServerTick(float DeltaTime)
{

	if (GetCharacterMovement()->Velocity.Size() < 200.0f || GroundSlope < -0.1f)
	{
		EndSlide();
	}
	else if (GroundSlope >= -0.1f && GroundSlope <= 0.1f)
	{
		// Allow sliding on flat surfaces for a short duration or if we are no longer moving on the ground end the slide
		float SlideDuration = 0.6f; 
		if (GetWorld()->GetTimeSeconds() - SlideStartTime >= SlideDuration || !GetMovementComponent()->IsMovingOnGround())
		{
			EndSlide();
		}
	}


}

void AFPSCharacter::StartADS(const FInputActionValue& InputActionValue)
{
	if (bMenuOpen || CurrentWeapon->bIsRPG) return;
		ServerSetADS(InputActionValue.Get<bool>());
	
}

void AFPSCharacter::ManageScoreboard(const FInputActionValue& InputActionValue)
{
	if (GetPlayerHUD())
	{
		UE_LOG(LogTemp, Warning, TEXT("Calling ManageScoreboard"));

		bScoreboardOpen = InputActionValue.Get<bool>();

		bScoreboardOpen ? GetPlayerHUD()->ShowScoreboard() : GetPlayerHUD()->HideScoreboard();
	}
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	APlayerController* PC = Cast<APlayerController>(GetController());
	if(PC)
	{
		 UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if(Subsystem)
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &AFPSCharacter::Move);
	PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &AFPSCharacter::Look);
	PEI->BindAction(InputActions->InputFire, ETriggerEvent::Triggered, this, &AFPSCharacter::StartFire);
	PEI->BindAction(InputActions->InputFire, ETriggerEvent::Completed, this, &AFPSCharacter::StopFire);
	PEI->BindAction(InputActions->InputInteraction, ETriggerEvent::Triggered, this, &AFPSCharacter::Interact);
	PEI->BindAction(InputActions->InputSwapWeapons, ETriggerEvent::Triggered, this, &AFPSCharacter::WeaponSwap);
	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Triggered, this, &AFPSCharacter::StartSprint);
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Triggered, this, &AFPSCharacter::Jump);
	PEI->BindAction(InputActions->InputCrouch, ETriggerEvent::Triggered, this, &AFPSCharacter::StartCrouch);
	PEI->BindAction(InputActions->InputADS, ETriggerEvent::Triggered, this, &AFPSCharacter::StartADS);
	PEI->BindAction(InputActions->InputReload, ETriggerEvent::Triggered, this, &AFPSCharacter::StartReload);
	PEI->BindAction(InputActions->InputTaccom, ETriggerEvent::Triggered, this, &AFPSCharacter::OpenTaccom);
	PEI->BindAction(InputActions->InputScoreboard, ETriggerEvent::Triggered, this, &AFPSCharacter::ManageScoreboard);
	PEI->BindAction(InputActions->InputTactical, ETriggerEvent::Triggered, this, &AFPSCharacter::StartTactical);
	PEI->BindAction(InputActions->InputMenu, ETriggerEvent::Triggered, this, &AFPSCharacter::OpenMenu);
	PEI->BindAction(InputActions->InputTeleportBack, ETriggerEvent::Triggered, this, &AFPSCharacter::StartTeleport);
}

void AFPSCharacter::StartFire(const FInputActionValue& Val)
{

	if (CanFire()) {
		if (CurrentWeapon) {
			if(CurrentWeapon->MuzzleFlash)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CurrentWeapon->MuzzleFlash, WeaponMesh->GetSocketLocation("MuzzleFlash"));
			}
			bCanFire = false;// Disallow firing
			bIsFiring = true;
			FTransform SocketTransform = WeaponMesh->GetSocketTransform("frontSightPost");

			if (HasAuthority())
			{
				Fire(SocketTransform);
			}
			else
			{
				ServerStartFire(SocketTransform);
			}
			// Only set a timer for automatic weapons
			if (CurrentWeapon->bIsAutomatic) {
				GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AFPSCharacter::ResetFire, CurrentWeapon->FireRate, false);
			}
		
		}
	}

}


void AFPSCharacter::StopFire()
{
	if (CurrentWeapon && CurrentWeapon->bIsAutomatic) {
		// For automatic weapons, clear the fire timer when the button is released
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);

		// And immediately allow firing again
		bCanFire = true;
	}
	else if (CurrentWeapon) {
		// For non-automatic weapons, reset fire immediately when the button is released
		ResetFire();
	}
}

void AFPSCharacter::ResetFire()
{
	// Allow firing again when the timer expires, regardless of whether the weapon is automatic or not
	bCanFire = true;
}

void AFPSCharacter::Fire(FTransform SocketTransform)
{
	FVector FireLocation;
	FRotator FireRotation;
	FRotator RandomSpread = FRotator(FMath::RandRange(-CurrentWeapon->WeaponSpread, CurrentWeapon->WeaponSpread), FMath::RandRange(-CurrentWeapon->WeaponSpread, CurrentWeapon->WeaponSpread), FMath::RandRange(-CurrentWeapon->WeaponSpread, CurrentWeapon->WeaponSpread));

	if (bIsADS)
	{
		// Get the socket location
		FireLocation = SocketTransform.GetLocation();

		// Get the socket rotation
		FireRotation = SocketTransform.GetRotation().Rotator();

		OriginalSocketRotation = WeaponMesh->GetRelativeRotation();

	}
	else
	{
		FTransform FiringPosition = GetFiringPosition();
		FireLocation = FiringPosition.GetLocation();
		FireRotation = FiringPosition.GetRotation().Rotator() + RandomSpread;
	}

	if (CurrentWeapon->bIsShotgun)
	{
		for (int i = 0; i < CurrentWeapon->ShotgunPellets; i++)
		{
			float RandomYaw = FMath::FRandRange(-CurrentWeapon->WeaponSpread, CurrentWeapon->WeaponSpread);
			float RandomPitch = FMath::FRandRange(-CurrentWeapon->WeaponSpread, CurrentWeapon->WeaponSpread);

			FRotator PelletRotation = FireRotation;
			PelletRotation.Yaw += RandomYaw;
			PelletRotation.Pitch += RandomPitch;

			ServerSpawnProjectile(FireLocation, PelletRotation);
		}


	}
	else
	{
		// For non-shotgun weapons, just spawn a single projectile with no spread
		ServerSpawnProjectile(FireLocation, FireRotation);

	}
	CurrentWeapon->DownwardKick = -CurrentWeapon->DownwardKickAmount;


	CurrentWeapon->AmmoInClip = FMath::Clamp(CurrentWeapon->AmmoInClip - 1, 0, CurrentWeapon->MaxAmmoInClip);

}

void AFPSCharacter::ServerStartTactical_Implementation()
{
	FVector FireLocation = FPSCameraComponent->GetComponentLocation() + GetActorForwardVector();
	FRotator Rotation = GetActorRotation();
	SpawnGrenade(FireLocation, Rotation);
}

bool AFPSCharacter::ServerStartTactical_Validate() {
	return true;
}

void AFPSCharacter::StartTactical(const FInputActionValue& Val)
{
	ServerStartTactical();
}

void AFPSCharacter::ServerStartFire_Implementation(FTransform SocketTransform)
{
	Fire(SocketTransform);
}

bool AFPSCharacter::ServerStartFire_Validate(FTransform SocketTransform)
{
	return true;
}
float AFPSCharacter::GetHealth()
{
	return HP;
}

float AFPSCharacter::GetShield()
{
	return Shield;
}
//remove this
void AFPSCharacter::UpdateHealth()
{
	
}

void AFPSCharacter::UpdateShield(float SP)
{

}

void AFPSCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip != nullptr && HasAuthority()) {

		if (WeaponToEquip->GetSpawnPoint())
		{
			WeaponToEquip->GetSpawnPoint()->OnWeaponPickedUp();
		}

		// If we have less than 2 weapons, just add the new weapon to the array
		if (EquippedWeapons.Num() < 2)
		{
			EquippedWeapons.Add(WeaponToEquip);
		}
		else
		{
			// We already have two weapons. Drop the current one and replace it with the new one.
			int32 CurrentWeaponIndex = EquippedWeapons.IndexOfByKey(CurrentWeapon);

			//if we already have 2 weapons we drop the current weapon.
			if (HasAuthority()) {
				DropWeapon(CurrentWeapon);
			}
			else
			{
				ServerDropWeapon(CurrentWeapon);
			}

			// Replace the current weapon in the array with the new weapon
			EquippedWeapons[CurrentWeaponIndex] = WeaponToEquip;
		}


				UseWeapon(WeaponToEquip);
				OnWeaponEquipped(WeaponToEquip);

	}

}

void AFPSCharacter::ServerUseWeapon_Implementation(AWeapon*Weapon)
{
	UseWeapon(Weapon);
}

bool AFPSCharacter::ServerUseWeapon_Validate(AWeapon* Weapon)
{
	return CurrentWeapon != nullptr && Weapon != nullptr;
}


void AFPSCharacter::UseWeapon(AWeapon* Weapon)
{
	//if weapon is null return
	if (!Weapon) return;

	//if we dont have authority ask the server
	if (!HasAuthority()) {
		ServerUseWeapon(Weapon);
	}

	//set the current weapon
	CurrentWeapon = Weapon;
	//attach the weapon to the character
	AttachWeaponToCharacter(Weapon);


}

void AFPSCharacter::AttachWeaponToCharacter(AWeapon* Weapon)
{

	if (!Weapon) return;

	Weapon->AttachToComponent(thirdPersonPlayerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Weapon");
	Weapon->bIsAttached = true;
	Weapon->SetActorHiddenInGame(false);
	if (PreviousWeapon) {
		PreviousWeapon->SetActorHiddenInGame(true);
	}
	SetFPSMesh(Weapon);
}


void AFPSCharacter::OnWeaponEquipped(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip) {
		WeaponToEquip->SetOwner(this);
		if (!HasAuthority() && IsLocallyControlled()) {
			WeaponToEquip->ServerSetPickUp(true);
		}
		else
		{
			WeaponToEquip->SetPickUp(true);
		}
	}
}


void AFPSCharacter::ServerEquipWeapon_Implementation(AWeapon* WeaponToEquip)
{
	EquipWeapon(WeaponToEquip);
}

bool AFPSCharacter::ServerEquipWeapon_Validate(AWeapon* WeaponToEquip)
{
	return true;
}



AWeapon* AFPSCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

APlayerHUD* AFPSCharacter::GetPlayerHUD()
{
	
		if (APlayerHUD* PlayerHUD = Cast<APlayerHUD>(GetWorld()->GetFirstPlayerController()->GetHUD())) {
			return PlayerHUD;
		}
	return nullptr;
}
float AFPSCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	
		LastDamagingPlayer = EventInstigator;

	

		if (GetShield() > 0) {
			float damageApplied = GetShield() - DamageTaken;
			SetShield(damageApplied);
			SetCombatStatus(EventInstigator);
		
			if(AFPSPlayerController*PC = Cast<AFPSPlayerController>(GetController()))
			{
				PC->ClientUpdateShieldHUD(GetShield()/MaxShield);
			}
			return damageApplied;
		}

		if (AFPSPlayerController* PC = Cast<AFPSPlayerController>(GetController()))
		{
			PC->ClientPerformFleshFlash();
		}

		float damageApplied = GetHealth() - DamageTaken;
		SetHealth(damageApplied);
		SetCombatStatus(EventInstigator);
		
		if (HasAuthority()) {

			for (FHitResult& HitResult : HitResults)
			{
				FBoneDamageInfo& BoneDamageInfo = BoneDamageThreshold.FindOrAdd(HitResult.BoneName);
				BoneDamageInfo.TotalDamage += DamageTaken;
				//UE_LOG(LogTemp, Warning, TEXT("%s : %f , %f"), *HitResult.BoneName.ToString(), BoneDamageInfo.TotalDamage, BoneDamageInfo.DamageThreshold);
				if (GetHealth() < 1) {
					if (BoneDamageInfo.TotalDamage >= BoneDamageInfo.DamageThreshold)
					{
						thirdPersonPlayerMesh->HideBoneByName(HitResult.BoneName, PBO_None);
						UE_LOG(LogTemp, Warning, TEXT("%s : %f , %f"), *HitResult.BoneName.ToString(), BoneDamageInfo.TotalDamage, BoneDamageInfo.DamageThreshold);

						if (BloodSplatter) {
							//spawn blood decal for gore
							SpawnDismembermentBlood(HitResult);
						}
					}
				}

			}
		}

		if (HasAuthority()) {
			if (HP <= 0)
			{
				KilledBy(EventInstigator);
				if (CurrentWeapon->GetPickedUp()) {
					DropWeapon(CurrentWeapon);
				}

			}
		}

		return damageApplied;

}

UCameraComponent* AFPSCharacter::GetCamera()
{
	return FPSCameraComponent;
}

void AFPSCharacter::Teleport()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(RecallTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(RecallTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Stopped Recall Timer in Teleport function"));
	}
	if(FPSPC)
	{
		FPSPC->ClientHideRecallBar();
		FPSPC->ClientUpdateRecallProgressBar(0);

	}
	if (HasAuthority()) {
		SetActorLocation(RecallLocation, false);
	}
	else
	{
		ServerTeleport();
	}
}

void AFPSCharacter::ServerPlayFootStep_Implementation(const FVector& Location)
{
	NetMulticastPlayFootStep(Location);
}

bool AFPSCharacter::ServerPlayFootStep_Validate(const FVector& Location)
{
	return true;
}

void AFPSCharacter::NetMulticastPlayFootStep_Implementation(const FVector& Location)
{
	DetermineSurfaceAndPlaySound(Location);
}

void AFPSCharacter::PlayFootStep(const FVector& Location)
{
	if(HasAuthority())
	{
		DetermineSurfaceAndPlaySound(Location);
	}
	else
	{
		ServerPlayFootStep(Location);
	}
}

void AFPSCharacter::DetermineSurfaceAndPlaySound(const FVector& Location)
{
	FVector Start = GetActorLocation();

	FVector End = Start - FVector(0.f,0.f,100.f);
	FHitResult hit;
	FCollisionQueryParams params;
	params.bReturnPhysicalMaterial = true;
	params.AddIgnoredActor(this);

	if(GetWorld()->LineTraceSingleByChannel(hit,Start,End,ECC_Visibility,params))
	{

		if(UPhysicalMaterial* PhysicalMaterial = hit.PhysMaterial.Get())
		{
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(PhysicalMaterial);
			switch(SurfaceType)
			{
			case SurfaceType1:
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), GroundCue, Location);
				break;
			case SurfaceType2:
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), WoodCue, Location);
				break;
			case SurfaceType3:
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), MetalCue, Location);
				break;
			}
		}
	}
}



void AFPSCharacter::ServerTeleport_Implementation()
{
	Teleport();
}

bool AFPSCharacter::ServerTeleport_Validate()
{
	return true;
}

void AFPSCharacter::SpawnDismembermentBlood_Implementation(const FHitResult& Hit)
{
	thirdPersonPlayerMesh->HideBoneByName(Hit.BoneName, PBO_None);
	UNiagaraFunctionLibrary::SpawnSystemAttached(BloodSplatter, thirdPersonPlayerMesh, Hit.BoneName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);

}


void AFPSCharacter::Jump()
{

	if (IsLookingAtWall())
	{
		if (ClimbCounter <= 3)
		{

			if (CurrentClimbingState == ClimbingState::NotClimbing && ClimbCounter == 0) {
				if (HasAuthority())
				{
					SetClimbing(ClimbingState::InitialClimb);
				}
				else
				{
					ServerSetClimbing(ClimbingState::InitialClimb);
				}
			}
			
		}
		else
		{
			if (HasAuthority())
			{
				SetClimbing(ClimbingState::NotClimbing);
			}
			else
			{
				ServerSetClimbing(ClimbingState::NotClimbing);
			}

		}
	}
	else
	{

		Super::Jump();
	}

}

void AFPSCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

		ClimbCounter = 0;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}


void AFPSCharacter::HandleReloadProgress(float DeltaTime)
{
	if (CurrentWeapon)
	{
		// Calculate the target rotation based on the reload progress
		FRotator TargetRotation = FRotator(DefaultWeaponTransform.GetRotation().Rotator().Pitch, DefaultWeaponTransform.GetRotation().Rotator().Yaw, ReloadProgress * 360.f);

		// Interpolate from the current rotation to the target rotation
		FRotator CurrentRotation = WeaponMesh->GetRelativeRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, CurrentWeapon->ReloadSpeed);

		WeaponMesh->SetRelativeRotation(NewRotation);
	}
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate variables to all clients
	DOREPLIFETIME(AFPSCharacter, HP);
	DOREPLIFETIME(AFPSCharacter, Shield);
	DOREPLIFETIME(AFPSCharacter, CurrentWeapon);
	DOREPLIFETIME(AFPSCharacter, EquippedWeapons);
	DOREPLIFETIME(AFPSCharacter, PreviousWeapon);
	DOREPLIFETIME(AFPSCharacter, bSprint);
	DOREPLIFETIME(AFPSCharacter, bIsSliding);
	DOREPLIFETIME(AFPSCharacter, SlideStartTime);
	DOREPLIFETIME(AFPSCharacter, InitialSlideVelocity);
	DOREPLIFETIME(AFPSCharacter, bIsADS);
	DOREPLIFETIME(AFPSCharacter, WeaponCollided);
	DOREPLIFETIME(AFPSCharacter, CanPlant);
	DOREPLIFETIME(AFPSCharacter, BombInteractionType);
	DOREPLIFETIME(AFPSCharacter, LastDamagingPlayer);
	DOREPLIFETIME(AFPSCharacter, MaxShield);
	DOREPLIFETIME(AFPSCharacter, DeathEXP);
	DOREPLIFETIME(AFPSCharacter, DeathGold);
	DOREPLIFETIME(AFPSCharacter, CurrentClimbingState);
	DOREPLIFETIME(AFPSCharacter, GrenadeWeapon);
	DOREPLIFETIME(AFPSCharacter, HitResults);
	DOREPLIFETIME(AFPSCharacter, WoodCue);
	DOREPLIFETIME(AFPSCharacter, GroundCue);
	DOREPLIFETIME(AFPSCharacter, MetalCue);
}
