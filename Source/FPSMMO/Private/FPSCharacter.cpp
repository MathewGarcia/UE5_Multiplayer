// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
#include "Engine.h"
#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneComponent.h"
#include "PlayerHUD.h"
#include "pHUD.h"
#include "FPSPlayerController.h"
#include "PlayerInfoState.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputConfigData.h"
#include "DrawDebugHelpers.h"
#include "PlayerCharacterMovementComponent.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{

	SetReplicates(true);
	bReplicates = true;
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
		// Set size for collision capsule
		GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

		// Create a follow camera
		FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
		FPSCameraComponent->SetupAttachment(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
		FPSCameraComponent->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
		
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

		MaxShield = 100;

	//set bcancrouch to true
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

		SlideStartTime = 0;

		DefaultFOV = 90;


}

void AFPSCharacter::Move(const FInputActionValue& Value)
{
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
	if (Controller != nullptr)
	{
		const FVector2D LookValue = Value.Get<FVector2D>();

		if (LookValue.X != 0.f)
		{
			AddControllerYawInput(LookValue.X);
		}

		if (LookValue.Y != 0.f)
		{
			AddControllerPitchInput(LookValue.Y);
		}
	}
}

void AFPSCharacter::SetCombatStatus(AController* EventInstigator)
{
	if (EventInstigator) {
		APlayerInfoState* instigatorPlayerState = Cast<APlayerInfoState>(EventInstigator->PlayerState);

		if (instigatorPlayerState)
		{
			instigatorPlayerState->player = Cast<AFPSCharacter>(instigatorPlayerState->GetPawn());
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
		if (Direction > 0)
		{
			//this means we're going up
			if (EquippedWeapons.Num() > 1)
			{
				if (CurrentWeapon != EquippedWeapons[1]) {
					PreviousWeapon = EquippedWeapons[0];
					UseWeapon(EquippedWeapons[1]);
				}
			}
		}
		else
		{
			if (EquippedWeapons.Num() > 1) {
				if (CurrentWeapon != EquippedWeapons[0]) {
					PreviousWeapon = EquippedWeapons[1];
					UseWeapon(EquippedWeapons[0]);
				}
			}


		}
	
}

void AFPSCharacter::OnRep_Sprint()
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

void AFPSCharacter::ApplyRecoil()
{

	if (CurrentWeapon)
	{
		FRotator Recoil = FRotator(FMath::FRandRange(-CurrentWeapon->MaxVerticalRecoil, CurrentWeapon->MaxVerticalRecoil),
			FMath::FRandRange(CurrentWeapon->MaxHorizontalRecoil, CurrentWeapon->MaxHorizontalRecoil), 0);


		if (Controller)
		{
			FRotator NewControlRotation = Controller->GetControlRotation() + Recoil;
			Controller->SetControlRotation(NewControlRotation);

		}
	
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
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic)); // ECC_GameTraceChannel1 as an example
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
				 UE_LOG(LogTemp, Warning, TEXT("Attempting to slide!"));
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
	FVector EndTrace = StartTrace - FVector(0.f, 0.f, 200.f); // 200 is an arbitrary value
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, QueryParams);

	GroundSlope = FVector::DotProduct(Hit.ImpactNormal, InitialSlideVelocity.GetSafeNormal());

	// Decrease speed due to friction
	float Friction = 200.0f; // adjust this value as necessary
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


	float CurrentSlideSpeed = GroundSlope * InitialSlideVelocity.Size() * 10;
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
	if (CurrentWeapon && CurrentWeapon->AmmoInClip != 0 && bCanFire && !CurrentWeapon->bIsReloading)
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
	MulticastDropWeapon(DroppedWeapon);
}

bool AFPSCharacter::ServerDropWeapon_Validate(AWeapon* DroppedWeapon)
{
	return true;
}

void AFPSCharacter::MulticastDropWeapon_Implementation(AWeapon* DroppedWeapon)
{
	FVector DropDirection = GetActorForwardVector();

	FVector DropLocation = GetActorLocation() + DropDirection * 10.0f; // 5 feet forward. Adjust this multiplier as per your requirement.

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

	DrawDebugLine(GetWorld(), DropLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
}

void AFPSCharacter::ServerPerformSlide_Implementation(float DeltaTime)
{

	ServerTick(DeltaTime);

}

bool AFPSCharacter::ServerPerformSlide_Validate(float DeltaTime)
{
	return true;
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	bCanFire = true;

	if (SpawningWeapon && HasAuthority()) {
			// Create an instance of the weapon object
			AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(SpawningWeapon);
			Weapon->SetActorLocation(GetActorLocation());
			EquipWeapon(Weapon);
	}
	

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		FPSPC = Cast<AFPSPlayerController>(PC);
	}

	if (FPSPC->IsLocalPlayerController())
	{
		HUDWidget = FPSPC->HUDWidget;
	}
	if (HasAuthority()) {
		SetShield(100);
		SetHealth(100);
	}

	DefaultWeaponTransform = FPSMeshArms->GetRelativeTransform();
	NormalWeaponRotation = WeaponMesh->GetRelativeRotation(); 


}

void AFPSCharacter::SetHealth(float hp)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		HP = hp;
		OnRep_Health();
	}

}

void AFPSCharacter::SetShield(float shield)
{
	if (GetLocalRole() == ROLE_Authority) {
		Shield = FMath::Clamp(shield, 0.0f, MaxShield);
		OnRep_Shield();
	}
	
}

void AFPSCharacter::OnRep_Health()
{
	UpdateHealth();
	if (GetHealth() <= 0)
	{
		KilledBy(LastDamagingPlayer);

	}
}

void AFPSCharacter::OnRep_Shield()
{
	UpdateShield(Shield);
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
			if (!PreviousWeapon) {
				OnRep_PreviousWeapon();
			}
			if (PreviousWeapon) {

				PreviousWeapon->SetActorHiddenInGame(true);
			}

			if (!CurrentWeapon)
			{
				if (EquippedWeapons.Num() > 2) {
					if (PreviousWeapon == EquippedWeapons[0])
					{
						CurrentWeapon = EquippedWeapons[1];
					}
					else
					{
						CurrentWeapon = EquippedWeapons[0];
					}
				}
				else
				{
					CurrentWeapon = EquippedWeapons[0];
				}
			}

		if (CurrentWeapon) {
			//if the current weapon is not attached to the player
			if (!CurrentWeapon->bIsAttached) {
				CurrentWeapon->AttachToComponent(thirdPersonPlayerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Weapon");
				CurrentWeapon->bIsAttached = true;
			}
			//hide previous weapon?
			//update mesh?
			CurrentWeapon->SetActorHiddenInGame(false);

			if (IsLocallyControlled())
			{
				CurrentWeapon->SetOwner(this);
				CurrentWeapon->WeaponMesh->SetOwnerNoSee(true);
				WeaponMesh->SetSkinnedAssetAndUpdate(Cast<USkinnedAsset>(CurrentWeapon->WeaponMesh->GetSkinnedAsset()));
				WeaponMesh->AttachToComponent(FPSMeshArms, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Weapon");
				
			}
		}
}

void AFPSCharacter::SpawnProjectile_Implementation(FVector SpawnLocation, FRotator SpawnRotation)
{
	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	FVector Offset = SpawnRotation.Vector() * 100;  // Change '100' to whatever offset distance you want
	SpawnLocation += Offset;
	SpawnRotation.Normalize();

	if (CurrentWeapon) {
		if (AProjectile* bullet = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, spawnParameters))
		{

			UPrimitiveComponent* BulletPrimitive = Cast<UPrimitiveComponent>(bullet->GetRootComponent());
			if (BulletPrimitive)
			{
				// Ignore the character
				BulletPrimitive->MoveIgnoreActors.Add(this);
			}

			bullet->SetInstigator(this);
			bullet->SetOwner(this);
			bullet->GetCollisionComponent()->IgnoreActorWhenMoving(this, true);
			FVector LaunchDirection = SpawnRotation.Vector();
			LaunchDirection *= CurrentWeapon->WeaponDistance;
			bullet->FireInDirection(LaunchDirection);


			// Calculate the end point of the bullet's trajectory
			FVector BulletTrajectoryEndPoint = SpawnLocation + (LaunchDirection * 10000); // Change this multiplier as needed depending on how far you want the trajectory to go

			// Draw the debug line following the bullet's trajectory
			DrawDebugLine(GetWorld(), SpawnLocation, BulletTrajectoryEndPoint, FColor::Red, true, 1, 0, 2);
		}

		CurrentWeapon->AmmoInClip = FMath::Clamp(CurrentWeapon->AmmoInClip - 1, 0, CurrentWeapon->MaxAmmoInClip);
		ApplyRecoil();
	}

}



FTransform AFPSCharacter::GetFiringPosition()
{
		// Calculate SpawnLocation and SpawnRotation as you were doing previously
		FVector SpawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 100.0f) + (GetActorUpVector() * 50.0f);
		FRotator SpawnRotation = GetControlRotation();

		return FTransform(SpawnRotation, SpawnLocation);

	
}


void AFPSCharacter::KilledBy(AController* EventInstigator)
{
	if (EventInstigator) {
		APlayerInfoState* instigatorPlayerState = Cast<APlayerInfoState>(EventInstigator->PlayerState);
		if (instigatorPlayerState)
		{
			instigatorPlayerState->UpdateEXP(10);
			instigatorPlayerState->UpdateGold(5);
		}
	}
}

void AFPSCharacter::Interact()
{
		HandleInteract();
}

void AFPSCharacter::HandleInteract_Implementation()
{
	if (GetLocalRole() == ROLE_Authority) {
		if (bInCollision)
		{
			//add the weapon to the equip
			if (WeaponCollided) {
				EquipWeapon(WeaponCollided);
				}
		}
	}
	else
	{
		if (bInCollision) {
			if (WeaponCollided)
				ServerEquipWeapon(WeaponCollided);
		}
	}
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	float CrouchInterpTime = FMath::Min(1.f, CrouchSpeed * DeltaTime);
	CrouchEyeOffset = (1.f - CrouchInterpTime) * CrouchEyeOffset;


	
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

}

void AFPSCharacter::ServerTick(float DeltaTime)
{

	if (GetCharacterMovement()->Velocity.Size() < 200.0f || GroundSlope < -0.1f)
	{
		EndSlide();
	}
	else if (GroundSlope >= -0.1f && GroundSlope <= 0.1f)
	{
		// Allow sliding on flat surfaces for a short duration
		float SlideDuration = 1.0f; // Adjust this value as necessary
		if (GetWorld()->GetTimeSeconds() - SlideStartTime >= SlideDuration)
		{
			EndSlide();
		}
	}

}

void AFPSCharacter::StartADS(const FInputActionValue& InputActionValue)
{
		ServerSetADS(InputActionValue.Get<bool>());
	
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
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	PEI->BindAction(InputActions->InputCrouch, ETriggerEvent::Triggered, this, &AFPSCharacter::StartCrouch);
	PEI->BindAction(InputActions->InputADS, ETriggerEvent::Triggered, this, &AFPSCharacter::StartADS);
	PEI->BindAction(InputActions->InputReload, ETriggerEvent::Triggered, this, &AFPSCharacter::StartReload);

}

void AFPSCharacter::StartFire(const FInputActionValue& Val)
{
	if (CanFire()) {
		if (CurrentWeapon) {
			bCanFire = false;  // Disallow firing
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
	//or statement may be redundant as it is never checked.
	if (bIsADS)
	{
		// Get the socket location
		FVector SocketLocation = SocketTransform.GetLocation();

		// Get the socket rotation
		FRotator SocketRotation = SocketTransform.GetRotation().Rotator();

		SpawnProjectile(SocketLocation, SocketRotation);
		return;
	}

	FTransform FiringPosition = GetFiringPosition();
	// If not aiming down sights, spawn the projectile immediately.
	SpawnProjectile(FiringPosition.GetLocation(), FiringPosition.GetRotation().Rotator());
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
void AFPSCharacter::UpdateHealth()
{
	if (IsLocallyControlled()) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Health: " + FString::Printf(TEXT("%f"), GetHealth())));

		}

		if (FPSPC)
		{
			float NHP = GetHealth();
			FPSPC->UpdateHP(NHP);
		}

		if(GetHealth() <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("You are Dead."));
		
		}
	}
	
}

void AFPSCharacter::UpdateShield(float SP)
{
	if (IsLocallyControlled()) {

		Shield = SP;

		if (GEngine) {

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("You have  " + FString::Printf(TEXT("%f shield remaining"), GetShield())));

		}


		if (FPSPC)
		{
			float NSP = GetShield();
			FPSPC->UpdateShield(NSP);
		}
	}
}

void AFPSCharacter::EquipWeaponOnServer(AWeapon* Weapon)
{
	if (Weapon != nullptr) {
		//Weapon->SetOwner(this);

		// If we have less than 2 weapons, just add the new weapon to the array
		if (EquippedWeapons.Num() < 2)
		{
			EquippedWeapons.Add(Weapon);
		}
		else
		{
			// We already have two weapons. Drop the current one and replace it with the new one.
			int32 CurrentWeaponIndex = EquippedWeapons.IndexOfByKey(CurrentWeapon);

			if (HasAuthority()) {
				MulticastDropWeapon(CurrentWeapon);
			}
			else {
				ServerDropWeapon(CurrentWeapon);
			}

			// Replace the current weapon in the array with the new weapon
			EquippedWeapons[CurrentWeaponIndex] = Weapon;
		}

		// Now use the weapon
		UseWeapon(Weapon);
		MulticastOnWeaponEquipped(Weapon);
	}
}

void AFPSCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(HasAuthority())
	{
	EquipWeaponOnServer(WeaponToEquip);
	}
	else
	{
		ServerEquipWeapon(WeaponToEquip);
	}
}




void AFPSCharacter::UseWeapon(AWeapon* Weapon)
{
	if (GetLocalRole() == ROLE_Authority) {
		CurrentWeapon = Weapon;
		OnRep_CurrentWeapon();
	}

}

void AFPSCharacter::ServerEquipWeapon_Implementation(AWeapon* WeaponToEquip)
{
	EquipWeaponOnServer(WeaponToEquip);
}

bool AFPSCharacter::ServerEquipWeapon_Validate(AWeapon* WeaponToEquip)
{
	return true;
}


void AFPSCharacter::MulticastOnWeaponEquipped_Implementation(AWeapon* WeaponToEquip)
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

void AFPSCharacter::SetClientSideWeapon(AWeapon* Weapon)
{
	if (Weapon)
	{
		//set the weapon mesh
		WeaponMesh = Weapon->WeaponMesh;
	}
}

AWeapon* AFPSCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

APlayerHUD* AFPSCharacter::GetPlayerHUD()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->ClientSetHUD(APlayerHUD::StaticClass());
		APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PC->GetHUD());
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
			return damageApplied;
		}
		else
		{
			float damageApplied = GetHealth() - DamageTaken;
			SetHealth(damageApplied);
			SetCombatStatus(EventInstigator);

			return damageApplied;
		}

	
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
}
