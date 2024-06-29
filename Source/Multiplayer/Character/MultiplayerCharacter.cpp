// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Multiplayer/ActorComponents/SM_AC_ControllerSettings.h"
#include "Perception/AISense_Damage.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMultiplayerCharacter

AMultiplayerCharacter::AMultiplayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	SpringArmInitialLocation = CameraBoom->GetRelativeLocation();
	MeshFP->HideBoneByName(FName("head"), PBO_None);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	
}

void AMultiplayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AMultiplayerCharacter::AddRecoil()
{
	float Random = 0.0f;
	
	if (AC_InventorySystem->CurrentWeapon)
	{
		InterpSpeed = AC_InventorySystem->CurrentWeapon->RecoilInterpSpeed;

		Random = UKismetMathLibrary::RandomFloatInRange(AC_InventorySystem->CurrentWeapon->WeaponVerticalRecoilMin,
			AC_InventorySystem->CurrentWeapon->WeaponVerticalRecoilMax) * -1.0f;

		PitchAcum += Random;

		if (UKismetMathLibrary::RandomBool())
		{
			Random = UKismetMathLibrary::RandomFloatInRange(AC_InventorySystem->CurrentWeapon->WeaponHorizontalRecoilMin,
			AC_InventorySystem->CurrentWeapon->WeaponHorizontalRecoilMax);
		}
		else
		{
			Random = UKismetMathLibrary::RandomFloatInRange(AC_InventorySystem->CurrentWeapon->WeaponHorizontalRecoilMin,
			AC_InventorySystem->CurrentWeapon->WeaponHorizontalRecoilMax) * -1.0f;
		}

		YawAcum += Random;
		
	}
}

void AMultiplayerCharacter::SetPOV()
{
	if (!FMath::Abs(CurrentFOV - TargetFOV) < 0.1f)
	{
		if (AC_InventorySystem->CurrentWeapon)
		{

			CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()),
				AC_InventorySystem->CurrentWeapon->AimingInterSpeed);

			FollowCamera->SetFieldOfView(CurrentFOV);
		}
	}
}

float AMultiplayerCharacter::MovementRecoil(ASM_WeaponBase* Weapon)
{
	float Spread = 0.0f;

	if (!GetController()->PlayerState->bIsABot)
	{
		if (AC_PawnInfo->GetAiming())
		{
			if (!Weapon->bIsShotgun && bIsFirstShot)
			{
				return Spread;
			}
			else
			{
				if (GetCharacterMovement()->IsFalling())
				{
					Spread = Weapon->AimingSpread * 3.0f;
					return Spread;
				}
				else
				{
					Spread = Weapon->AimingSpread;
					return Spread;
				}
			}
		}
		else
		{
			float TemporarySpread_1 = 0.0f;
			if (bIsCrouched)
			{
				TemporarySpread_1 = Weapon->WeaponSpread * 0.6f;
			}
			else
			{
				TemporarySpread_1 = Weapon->WeaponSpread;
			}

			float TemporarySpread_2 = 0.0f;

			if (bIsCrouched)
			{
				TemporarySpread_2 = UKismetMathLibrary::VSize(UKismetMathLibrary::ClampVectorSize(GetVelocity(), 0.0, 0.03));
			}
			else
			{
				TemporarySpread_2 = UKismetMathLibrary::VSize(UKismetMathLibrary::ClampVectorSize(GetVelocity(), 0.0, 0.1));
			}

			//refactor
			Spread = (TemporarySpread_1 + (TemporarySpread_2 / 2.0f)) + TemporarySpread_1 + (TemporarySpread_2 / 2.0f) * 0.03f;
			return Spread;
		}
	}
	else
	{
		Spread = Weapon->AimingSpread;
		return Spread;
	}
	
}

void AMultiplayerCharacter::SetWeaponVisibility(ASM_WeaponBase* Weapon, bool Visible)
{
	if (Weapon)
	{
		Weapon->MeshFPP->SetVisibility(Visible, true);
		Weapon->MeshTPP->SetVisibility(Visible, true);
	}
}

void AMultiplayerCharacter::SetShooting(bool bShooting)
{
}

void AMultiplayerCharacter::StopShooting()
{
	GetWorld()->GetTimerManager().PauseTimer(AutoShootTimerHandle);
	SetShooting(false);
	TryRun();
}

void AMultiplayerCharacter::TryRun()
{
}

void AMultiplayerCharacter::AddRecoilPitch()
{
	if (ActualPitch != PitchAcum)
	{
		float TemporaryPitch = UKismetMathLibrary::FInterpTo(ActualPitch, PitchAcum, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), InterpSpeed);
		AddControllerPitchInput(ActualPitch - TemporaryPitch);
		ActualPitch = TemporaryPitch;
	}
}

void AMultiplayerCharacter::ResetRecoil()
{
	PitchAcum = 0.0f;
	YawAcum = 0.0f;
	bIsFirstShot = true;

	if (AC_InventorySystem->CurrentWeapon)
	{
		InterpSpeed = AC_InventorySystem->CurrentWeapon->RecoverInterpSpeed;
	}
}

void AMultiplayerCharacter::AddRecoilYaw()
{
	if (ActualYaw != YawAcum)
	{
		float TemporaryYaw = UKismetMathLibrary::FInterpTo(ActualYaw, YawAcum, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), InterpSpeed / 2.0f);
		AddControllerYawInput(ActualYaw - TemporaryYaw);
		ActualYaw = TemporaryYaw;
	}
}

void AMultiplayerCharacter::AdjustCamera(float Input)
{
	float Value = UKismetMathLibrary::DegSin(10.0f) * Input;

	AddControllerYawInput(Value);
}

void AMultiplayerCharacter::OnDamageTaken(float Damage, UDamageType* DamageType, AController* InstigatorController,
	AActor* DamageCauser)
{
	if (!AC_Health->IsDead())
	{
		if (Damage > 0.0f)
		{
			// void DamagePlayer();
		}
		else
		{
			//void HealPlayer();
		}
	}
}

void AMultiplayerCharacter::MeleeAttack()
{
	TArray<AActor*> IgnoreActor;
	IgnoreActor.Add(this);

	FHitResult* HitResult = nullptr;

	FVector StartLocation = FVector::Zero();
	FVector EndLocation = FVector::Zero();

	USM_AC_ControllerSettings* ControllerSettings = Cast<USM_AC_ControllerSettings>(GetController()->GetComponentByClass(USM_AC_ControllerSettings::StaticClass())); 
	if (ControllerSettings->GetUseTPP())
	{
		StartLocation = GetActorLocation();
	}
	else
	{
		StartLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetRootComponent()->GetComponentLocation();
	}

	EndLocation = StartLocation + UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetRootComponent()->GetComponentLocation() * MeleeAttackRange;

	UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), StartLocation, EndLocation, 35.0f, 0.0f,
		TraceTypeQuery1, false, IgnoreActor, EDrawDebugTrace::None, *HitResult, true);

	if (HitResult->GetActor())
	{
		USM_AC_Health* Health = Cast<USM_AC_Health>(HitResult->GetActor()->GetComponentByClass(USM_AC_Health::StaticClass()));
		if (Health)
		{
			// Apply Damage
		}
	}
}

void AMultiplayerCharacter::OnDead(AActor* DeadActor, AController* InstigatorController, UDamageType* DamageType,
	AActor* DamageCauser)
{
	SetCanBeDamaged(false);
	bIsWantsToRun = false;
	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->StopMovementImmediately();
	/*if (InstigatorController)
	{
		
	}*/

	FollowCamera->SetFieldOfView(DefaultFOV);
	AC_PawnInfo->SetRunning(false);

	GetWorld()->GetTimerManager().PauseTimer(AutoShootTimerHandle);

	ACharacter Character* = Cast<ACharacter>(DeadActor);

	Character->GetCharacterMovement()->DisableMovement();

	AC_InventorySystem->Server_DropWeapon();
	AC_InventorySystem->DestroyWeapons();

	// Create GameState Class
	AGameStateBase* GameStateBase = UGameplayStatics::GetGameState(GetWorld());

	
}

void AMultiplayerCharacter::OnTurn(float Input)
{
	float Value = 0.0f;

	USM_AC_ControllerSettings* ControllerSettings = Cast<USM_AC_ControllerSettings>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetComponentByClass(USM_AC_ControllerSettings::StaticClass()));

	bool VerticalAxis = false;
	bool HorizontalAxis = false;
	float MouseSens = 0.0f;
	float AimSens = 0.0f;

	ControllerSettings->GetMouseSettings(VerticalAxis, HorizontalAxis, MouseSens, AimSens);

	if (HorizontalAxis)
	{
		Value = ReduceAimingSensitivity(Input * -1.0f, MouseSens, AimSens);
	}
	else
	{
		Value = ReduceAimingSensitivity(Input, MouseSens, AimSens);
	}

	AddControllerYawInput(Value);

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	if (TimerManager.IsTimerActive(AutoShootTimerHandle))
	{
		ActualYaw = 0.0f;
	}
	
}

void AMultiplayerCharacter::OnLookUp(float Input)
{
	float Value = 0.0f;

	USM_AC_ControllerSettings* ControllerSettings = Cast<USM_AC_ControllerSettings>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetComponentByClass(USM_AC_ControllerSettings::StaticClass()));

	bool VerticalAxis = false;
	bool HorizontalAxis = false;
	float MouseSens = 0.0f;
	float AimSens = 0.0f;

	ControllerSettings->GetMouseSettings(VerticalAxis, HorizontalAxis, MouseSens, AimSens);

	if (VerticalAxis)
	{
		Value = ReduceAimingSensitivity(Input * -1.0f, MouseSens, AimSens);
	}
	else
	{
		Value = ReduceAimingSensitivity(Input, MouseSens, AimSens);
	}

	if (!AC_Health->IsDead())
	{
		AdjustCamera(Value);
		AddControllerPitchInput(Value);

		
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
		if (TimerManager.IsTimerActive(AutoShootTimerHandle))
		{
			if (PitchAcum + Value > 0.0f && PitchAcum == 0.0f)
			{
				ResetRecoil();
			}
			else
			{
				if (!PitchAcum > 0.0f)
				{
					PitchAcum += Value;
				}
				else
				{
					PitchAcum = 0.0f;
					ActualPitch += Value;
				}
				
			}
		}
		else
		{
			if (Value != 0.0f)
			{
				ResetRecoil();
				ActualPitch = 0.0f;
				PitchAcum = 0.0f;
			}
		}
	}
}

float AMultiplayerCharacter::ReduceAimingSensitivity(float Input, float MouseSensitivity, float AimSensitivity)
{
	if (AC_PawnInfo->GetAiming())
	{
		return Input * MouseSensitivity * AimSensitivity;
	}
	
	return Input * MouseSensitivity; 
	
}

void AMultiplayerCharacter::OnMoveForward(float Input)
{
	InputValue = Input;
	AddMovementInput(GetActorForwardVector(), InputValue, false);
}

void AMultiplayerCharacter::OnMoveRight(float Input)
{
	AddMovementInput(GetActorRightVector(), Input, false);
}

void AMultiplayerCharacter::UpdatePerspective()
{
	// Arms update event
}

void AMultiplayerCharacter::ChangeFiringMode()
{
	if (bIsCanShoot && !bIsFiringBurst && !bIsPerformingAction && !bIsChangingWeapon && bIsFireEnabled)
	{
		if (AC_InventorySystem->CurrentWeapon)
		{
			if (AC_InventorySystem->CurrentWeapon->FiringModes.Num() >= 2)
			{
				AC_InventorySystem->CurrentWeapon->ChangeFiringMode();
				USM_AC_ControllerSettings* ControllerSettings = Cast<USM_AC_ControllerSettings>(GetController()->GetComponentByClass(USM_AC_ControllerSettings::StaticClass())); 
				ControllerSettings->AddFiringModeUsed(AC_InventorySystem->CurrentWeapon->WeaponName, AC_InventorySystem->CurrentWeapon->ActualFiringMode);
				// Sound
				AC_InventorySystem->OnFiringModeChanged.Broadcast();
			}
		}
	}
}

void AMultiplayerCharacter::PawnDeath()
{
	if (!GetPlayerState()->IsABot())
	{
		// Stop Aiming
	}

	if (UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
	{
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StopAllCameraShakes(true);
	}

	GetWorld()->GetTimerManager().PauseTimer(AutoShootTimerHandle);
}

void AMultiplayerCharacter::UpdateArmsVisibility()
{
	USM_AC_ControllerSettings* ControllerSettings = Cast<USM_AC_ControllerSettings>(GetController()->GetComponentByClass(USM_AC_ControllerSettings::StaticClass()));
	if (ControllerSettings)
	{
		MeshFP->SetVisibility(!ControllerSettings->GetUseTPP(), false);
		GetMesh()->SetOwnerNoSee(!ControllerSettings->GetUseTPP());

		if (AC_InventorySystem->CurrentWeapon)
		{
			AC_InventorySystem->CurrentWeapon->UpdateWeaponVisibility(ControllerSettings->GetUseTPP());
		}

		if (AC_InventorySystem->LethalEquipment)
		{
			// Add Mesh for LethalEquipment
		}
	}
}

void AMultiplayerCharacter::UpdateSpringArmPosition()
{
	if (GetController())
	{
		USM_AC_ControllerSettings* ControllerSettings = Cast<USM_AC_ControllerSettings>(GetController()->GetComponentByClass(USM_AC_ControllerSettings::StaticClass()));

		if (ControllerSettings)
		{
			if (ControllerSettings->GetUseTPP())
			{
				CameraBoom->TargetArmLength = TPPSpringArmLength;
			}
			else
			{
				CameraBoom->TargetArmLength = 0.0f;
			}

			if (ControllerSettings->GetUseTPP())
			{
				if (FMath::Abs(CameraBoom->GetRelativeLocation().X - 0.0f) < 0.01f)
				{
					FVector FinalLocation = FVector(CameraBoom->GetRelativeLocation().X, CameraBoom->GetRelativeLocation().Y + 30.0f, CameraBoom->GetRelativeLocation().Z);
					CameraBoom->SetRelativeLocation(FinalLocation);
				}
			}
			else
			{
				CameraBoom->SetRelativeLocation(SpringArmInitialLocation);
			}
		}
	}
}

void AMultiplayerCharacter::ResetCollision()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	GetMesh()->SetCollisionProfileName(FName("Pawn"), true);
	SetActorEnableCollision(true);
}

void AMultiplayerCharacter::HealPlayer(float HealAmount, AController* HealInstigator)
{
	float HPToHeal = 0.0f;
	
	if (HealAmount <= AC_Health->GetMaxHealth() - AC_Health->GetHealth())
	{
		HPToHeal = HealAmount;
	}
	else
	{
		HPToHeal = AC_Health->GetMaxHealth() - AC_Health->GetHealth();
	}

	if (HealInstigator)
	{
		if (HPToHeal >= 0.0f)
		{
			// Add Point to Instigator
		}
	}

	AC_Health->HandleDamage(HPToHeal * -1.0f);
}

void AMultiplayerCharacter::DamagePlayer(float Damage, UDamageType* DamageType, AController* InstigatorController,
	AActor* DamageCauser)
{
	if (!AC_Health->IsDead())
	{
		// Play Hit
		
		float DamageToDo = TakeDamage(Damage, InstigatorController);

		if (DamageToDo > 0.0f)
		{
			// Add Directional Damage ()

			UAISense_Damage::ReportDamageEvent(GetWorld(), this, DamageCauser, DamageToDo,
				DamageCauser->GetActorLocation(), FVector::Zero(), NAME_None);

			if (AC_Health->IsDead())
			{
				if (GetController() == InstigatorController)
				{
					// Suicide
				}
				else
				{
					// Add kill to team
				}

				HandleDeath(this, InstigatorController, DamageType, DamageCauser);
			}
		}
	}
}

float AMultiplayerCharacter::TakeDamage(float DamageAmount, AController* ControllerInstigator)
{
	
}

void AMultiplayerCharacter::HandleDeath(AActor* Dead_Actor, AController* InstigatorController, UDamageType* DmgType,
	AActor* DamageCauser)
{
}

void AMultiplayerCharacter::ShowWeapon()
{
	
}


//////////////////////////////////////////////////////////////////////////
// Input

void AMultiplayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMultiplayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMultiplayerCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMultiplayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMultiplayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}