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
#include "PlayerCharacterMovementComponent.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{

	SetReplicates(true);
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
		// Set size for collision capsule
		GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

		// Create a follow camera
		FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
		FPSCameraComponent->SetupAttachment(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
		FPSCameraComponent->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
		
		// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
		FPSMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSMesh"));
		FPSMeshComponent->SetOnlyOwnerSee(true);
		FPSMeshComponent->SetupAttachment(FPSCameraComponent);
		FPSMeshComponent->bCastDynamicShadow = false;
		FPSMeshComponent->CastShadow = false;

		WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
		WeaponMesh->SetupAttachment(FPSCameraComponent);
		WeaponMesh->SetOnlyOwnerSee(true);


		playerMeshTest = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("playerMesh"));
		playerMeshTest->SetupAttachment(RootComponent);
		// Make sure the static mesh is visible
		playerMeshTest->SetVisibility(true);
		playerMeshTest->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		playerMeshTest->SetOnlyOwnerSee(false);
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
	UE_LOG(LogTemp, Warning, TEXT("Direciton: %f"), Val);
	SwitchWeapon(Val);
	
}

void AFPSCharacter::SwitchWeapon_Implementation(float Direction)
{
		if (Direction > 0)
		{
			//this means we're going up
			if (ClientWeapons.Num() > 1)
			{
				if (CurrentWeapon != ClientWeapons[1]) {
					PreviousWeapon = ClientWeapons[0];
					UseWeapon(ClientWeapons[1]);
					OnRep_CurrentWeapon();
				}
			}
		}
		else
		{
			if (ClientWeapons.Num() > 1) {
				if (CurrentWeapon != ClientWeapons[0]) {
					PreviousWeapon = ClientWeapons[1];
					UseWeapon(ClientWeapons[0]);
					OnRep_CurrentWeapon();
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

	FVector SlideDirection = InitialSlideVelocity.GetSafeNormal();
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
	FTransform CurrentTransform = WeaponMesh->GetRelativeTransform();
	FVector CurrentLocation = CurrentTransform.GetLocation();
	FQuat CurrentRotation = CurrentTransform.GetRotation();

	if (bIsADS)
	{
		// We are in ADS mode, interpolate towards ADS transform
		FVector TargetLocation = ADSWeaponTransform.GetLocation();
		FQuat TargetRotation = ADSWeaponTransform.GetRotation();

		FVector NewLocation = FMath::Lerp(CurrentLocation, TargetLocation, ADSInterpSpeed * DeltaTime);
		FQuat NewRotation = FQuat::Slerp(CurrentRotation, TargetRotation, ADSInterpSpeed * DeltaTime);

		WeaponMesh->SetRelativeLocation(NewLocation);
		WeaponMesh->SetRelativeRotation(NewRotation);
	}
	else
	{
		// We are not in ADS mode, interpolate towards default transform
		FVector TargetLocation = DefaultWeaponTransform.GetLocation();
		FQuat TargetRotation = DefaultWeaponTransform.GetRotation();

		FVector NewLocation = FMath::Lerp(CurrentLocation, TargetLocation, ADSInterpSpeed * DeltaTime);
		FQuat NewRotation = FQuat::Slerp(CurrentRotation, TargetRotation, ADSInterpSpeed * DeltaTime);

		WeaponMesh->SetRelativeLocation(NewLocation);
		WeaponMesh->SetRelativeRotation(NewRotation);
	}
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

	UObject* Asset = LoadObject<UObject>(nullptr, TEXT("/Game/PlayerStuff/BP_TestWeapon.BP_TestWeapon"));
	if (Asset)
	{
		if (Asset->IsA<UBlueprint>())
		{
			UBlueprint* BP = Cast<UBlueprint>(Asset);
			if (BP->GeneratedClass && BP->GeneratedClass->IsChildOf(AWeapon::StaticClass()))
			{
					// Create an instance of the weapon object
					AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(BP->GeneratedClass);
						Weapon->SetPickUp(true);
						EquipWeapon(Weapon);
						OnRep_CurrentWeapon();
				
			}
		
		}
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

	DefaultWeaponTransform = WeaponMesh->GetRelativeTransform();

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
	if (ClientWeapons.Num() > 2) {
		if (CurrentWeapon == ClientWeapons[1])
		{
			PreviousWeapon = ClientWeapons[0];
		}
		else
		{
			PreviousWeapon = ClientWeapons[1];
		}
	}
	else
	{
		PreviousWeapon = ClientWeapons[0];
	}
}

void AFPSCharacter::OnRep_EquippedWeapons()
{
	UpdateEquippedWeapons();
}

void AFPSCharacter::UpdateEquippedWeapons()
{
	ClientWeapons = EquippedWeapons;

	if(!CurrentWeapon)
	CurrentWeapon = EquippedWeapons.Num() > 0 ? EquippedWeapons[0] : nullptr;

}

void AFPSCharacter::OnRep_CurrentWeapon()
{
			if (!PreviousWeapon) {
				OnRep_PreviousWeapon();
			}

			PreviousWeapon->SetActorHiddenInGame(true);

		if (!CurrentWeapon)
		{
			if (ClientWeapons.Num() > 2) {
				if (PreviousWeapon == ClientWeapons[0])
				{
					CurrentWeapon = ClientWeapons[1];
				}
				else
				{
					CurrentWeapon = ClientWeapons[0];
				}
			}
			else
			{
				CurrentWeapon = ClientWeapons[0];
			}
		}

		if (CurrentWeapon) {
			//if the current weapon is not attached to the player
			if (!CurrentWeapon->bIsAttached) {
				CurrentWeapon->AttachToComponent(playerMeshTest, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Weapon");
				CurrentWeapon->bIsAttached = true;
				CurrentWeapon->SetOwner(this);
				CurrentWeapon->WeaponMesh->SetOwnerNoSee(true);
			}
			//hide previous weapon?
			//update mesh?
			CurrentWeapon->SetActorHiddenInGame(false);
			if (IsLocallyControlled())
			{
				WeaponMesh->SetStaticMesh(CurrentWeapon->WeaponMesh->GetStaticMesh());

			}
		}
}

void AFPSCharacter::HandleFire_Implementation()
{

	FRotator SpawnRotation = GetControlRotation();

		if (CurrentWeapon) {
			// Set the projectile's initial velocity
			if (Projectile)
			{
				FVector LaunchDirection = SpawnRotation.Vector();
				LaunchDirection *= CurrentWeapon->WeaponDistance;
				Projectile->FireInDirection(LaunchDirection);


				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("FIRING"));

			}

		}
}

void AFPSCharacter::SpawnProjectile_Implementation()
{

	// Spawn a projectile at the camera location
	FVector SpawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 100.0f) + (GetActorUpVector() * 50.0f);
	FRotator SpawnRotation = GetControlRotation();
	FActorSpawnParameters spawnParameters;

	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, spawnParameters);
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
				WeaponCollided->SetActorHiddenInGame(true);
				WeaponCollided->SetPickUp(true);
				EquipWeapon(WeaponCollided);
			}
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
	PEI->BindAction(InputActions->InputInteraction, ETriggerEvent::Triggered, this, &AFPSCharacter::Interact);
	PEI->BindAction(InputActions->InputSwapWeapons, ETriggerEvent::Triggered, this, &AFPSCharacter::WeaponSwap);
	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Triggered, this, &AFPSCharacter::StartSprint);
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	PEI->BindAction(InputActions->InputCrouch, ETriggerEvent::Triggered, this, &AFPSCharacter::StartCrouch);
	PEI->BindAction(InputActions->InputADS, ETriggerEvent::Triggered, this, &AFPSCharacter::StartADS);

}

void AFPSCharacter::StartFire(const FInputActionValue& Val)
{
	if (bCanFire) {
		if (CurrentWeapon) {
			bCanFire = false;
			// Set a timer to allow firing again
			GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AFPSCharacter::ResetFire, CurrentWeapon->FireRate, false);
			SpawnProjectile();
		}
	}
	
	
}

void AFPSCharacter::StopFire()
{
	// Clear the fire timer
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);

	// Reset the firing flag
	bCanFire = true;
}

void AFPSCharacter::ResetFire()
{
	StopFire();
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

void AFPSCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip != nullptr) {
		//if we dont have max weapons, add it to the equipped weapons.

			if (EquippedWeapons.Num() > 0 && EquippedWeapons.Num() < 2) {
				if (EquippedWeapons[0] == nullptr)
				{
					EquippedWeapons[0] = WeaponToEquip;
				}
				else {
					EquippedWeapons.Add(WeaponToEquip);
				}
			}
			else if(EquippedWeapons.IsEmpty())
			{
				EquippedWeapons.Add(WeaponToEquip);
			}
			//else we want to replace the current weapon with the new weapon
			else
			{

				//TODO:we want to drop the weapon 5ft away from the player towards the ground.
				AWeapon* DroppedWeapon = CurrentWeapon;
				FVector DropDirection = GetActorForwardVector();

				FVector DropLocation = GetActorLocation() + DropDirection * 5.0f; // 5 feet forward. Adjust this multiplier as per your requirement.

				FVector TraceEnd = DropLocation - FVector(0, 0, 500.0f); // 500 units down.

				FCollisionQueryParams QueryParams;

				QueryParams.AddIgnoredActor(this);
				QueryParams.bTraceComplex = true;

				FHitResult Hit;

				if(GetWorld()->LineTraceSingleByChannel(Hit, DropLocation, TraceEnd, ECC_Visibility))
				{

					DroppedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
					DroppedWeapon->SetActorLocation(Hit.Location);
					DroppedWeapon->SetPickUp(false);


					UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *Hit.GetActor()->GetName());
				}

				DrawDebugLine(GetWorld(), DropLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);



				if(CurrentWeapon == EquippedWeapons[0])
				{
					EquippedWeapons[0] = WeaponToEquip;
				}
				else
				{
					EquippedWeapons[1] = WeaponToEquip;
				}
				UseWeapon(WeaponToEquip);
			}

			OnRep_EquippedWeapons();

	}
	
}



void AFPSCharacter::UseWeapon(AWeapon* Weapon)
{
	if (GetLocalRole() == ROLE_Authority) {
		CurrentWeapon = Weapon;
		OnRep_CurrentWeapon();
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
}
