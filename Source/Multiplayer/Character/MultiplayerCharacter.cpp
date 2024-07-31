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
#include "Engine/DamageEvents.h"
#include "GameFramework/GameStateBase.h"
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
	//MeshFP->HideBoneByName(FName("head"), PBO_None);

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

void AMultiplayerCharacter::Tick(float DeltaSeconds)
{
	if (IsLocallyControlled())
	{
		AddRecoil();
		AddRecoilYaw();
	}

	if (HasAuthority())
	{
		if (GetPlayerState())
		{
			if (GetPlayerState()->IsABot())
			{
				GetMovementSpeed();
			}	
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
	
	if (!(FMath::Abs(CurrentFOV - TargetFOV) < 0.1f))
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

	if (!GetController()->PlayerState->IsABot())
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
	// Server SetShooting
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

	ACharacter* Character = Cast<ACharacter>(DeadActor);

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
				if (!(PitchAcum > 0.0f))
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

void AMultiplayerCharacter::HealPlayer(float NeedHealAmount, AController* HealInstigator)
{
	float HPToHeal = 0.0f;
	
	if (NeedHealAmount <= AC_Health->GetMaxHealth() - AC_Health->GetHealth())
	{
		HPToHeal = NeedHealAmount;
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

		FDamageEvent DamageEvent;
		
		float DamageToDo = TakeDamage(Damage, DamageEvent, InstigatorController, DamageCauser);

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

float AMultiplayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DmgToDo = DamageAmount;
	bool bIsSelfInflicted = EventInstigator == GetController();

	//Add logic
	if (EventInstigator)
	{
		if (!bIsSelfInflicted)
		{
			
		}

		if (bIsSelfInflicted)
		{
			AC_Health->HandleDamage(DmgToDo);
		}
		else
		{
			// Hit Marker
		}

		return DmgToDo;
	}

	return 0.0f;
}

void AMultiplayerCharacter::HandleDeath(AActor* Dead_Actor, AController* InstigatorController, UDamageType* DmgType,
	AActor* DamageCauser)
{
}

void AMultiplayerCharacter::ShowWeapon()
{
	if (AC_InventorySystem->CurrentWeapon)
	{
		USM_AC_ControllerSettings* ControllerSettings = Cast<USM_AC_ControllerSettings>(GetController()->GetComponentByClass(USM_AC_ControllerSettings::StaticClass()));
		if (ControllerSettings)
		{
			AC_InventorySystem->CurrentWeapon->UpdateWeaponVisibility(ControllerSettings->GetUseTPP());
		}
	}
}

void AMultiplayerCharacter::AttachWeapon(ASM_WeaponBase* WeaponCreated)
{
	WeaponCreated->SetOwner(this);

	FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	
	WeaponCreated->MeshFPP->AttachToComponent(MeshFP, AttachmentTransformRules, FName("head_r"));
	WeaponCreated->MeshTPP->AttachToComponent(GetMesh(), AttachmentTransformRules, FName("head_r"));
}

void AMultiplayerCharacter::SimulatedPhysicsOnWeapon(ASM_WeaponBase* Weapon, bool bIsSimulate)
{
	ECollisionEnabled::Type Collision = bIsSimulate ? ECollisionEnabled::Type::QueryAndPhysics : ECollisionEnabled::Type::NoCollision;
	Weapon->MeshTPP->SetCollisionEnabled(Collision);
	Weapon->MeshTPP->SetSimulatePhysics(bIsSimulate);
}

void AMultiplayerCharacter::StopCurrentWeaponReload()
{
	if (AC_InventorySystem->CurrentWeapon)
	{
		if (AC_InventorySystem->CurrentWeapon->AM_FPP_ReloadWeapon)
		{
			MeshFP->GetAnimInstance()->Montage_Stop(0.3f, AC_InventorySystem->CurrentWeapon->AM_FPP_ReloadWeapon);
		}
	}
}

bool AMultiplayerCharacter::bIsUsingPrimary()
{
	return AC_InventorySystem->PrimaryWeapon == AC_InventorySystem->CurrentWeapon;
}

void AMultiplayerCharacter::OnRep_bIsShooting()
{
	if (bIsShooting)
	{
		// Firing
	}
}

float AMultiplayerCharacter::CalculateMovementInput(float Input)
{
	float Output = 0.0f;
	switch (MovementType)
	{
	case Walk:
		Output = bIsCrouched ? 1.0f * Input : 0.625f * Input;
		break;
	case Run:
		Output = 1.0f * Input;
		break;
	case Aim:
		Output = bIsCrouched ? 1.0f * Input : 0.295f * Input;
	}

	return Output;
}

float AMultiplayerCharacter::ScaleAxisInput(float InputAxisValue)
{
	float Value = InputAxisValue > 0.0f ? FMath::Sqrt(FMath::Abs(FMath::Pow(InputAxisValue, 2) * GetMultiplier())) :
			FMath::Sqrt(FMath::Abs(FMath::Pow(InputAxisValue, 2) * GetMultiplier())) * -1.0f;

	return CalculateMovementInput(Value);
}

float AMultiplayerCharacter::GetMultiplier()
{
	// Refactor
	return  0.0f;
}

void AMultiplayerCharacter::Ragdoll()
{
	if (this)
	{
		this->SetActorEnableCollision(true);
		
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
		GetMesh()->SetCollisionProfileName(FName("Pawn"), true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->AddImpulse(UKismetMathLibrary::Normal(HitDirection, 0.0001f) * 5000.0f, HitBone, false);

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	}
}

TSoftClassPtr<ASM_WeaponBase> AMultiplayerCharacter::GetRandomWeaponToSpawn()
{
	TSoftClassPtr<ASM_WeaponBase> WeaponToSpawn = nullptr;

	WeaponToSpawn = Weapons[UKismetMathLibrary::RandomIntegerInRange(0, Weapons.Num() -1)];
	
	return WeaponToSpawn;
}

void AMultiplayerCharacter::GetMovementSpeed()
{
	if (AC_PawnInfo->GetAiming())
	{
		MovementType = Aim;
	}
	else
	{
		MovementType = Walk;
	}

	float WalkSpeed = 0.0f;

	switch (MovementType)
	{
	case Walk:
		WalkSpeed = GetCharacterMovement()->IsCrouching() ? 1.0f : 0.625f;
		break;
	case Run:
		WalkSpeed = 1.0f;
		break;
	case Aim:
		WalkSpeed = GetCharacterMovement()->IsCrouching() ? 1.0f : 0.295f;
	}
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * 576.0f;
}

void AMultiplayerCharacter::AITryRun()
{
	AC_PawnInfo->SetRunning(FMath::Abs(GetCharacterMovement()->Velocity.Length()) > 300.0f);
}

void AMultiplayerCharacter::SetWeaponAnimBlueprints(ASM_WeaponBase* Weapon)
{
	MeshFP->LinkAnimClassLayers(Weapon->FPP_WeaponAnims);
	GetMesh()->LinkAnimClassLayers(Weapon->TPP_WeaponAnims);
}

void AMultiplayerCharacter::EnableDamage()
{
	SetCanBeDamaged(true);
}

void AMultiplayerCharacter::PossessedBy(AController* NewController)
{
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("EnableDamage"));

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.0f, false);

	if (GetPlayerState()->IsABot())
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		PossessedEvent();
		
	}
}

void AMultiplayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	UpdateRun();
	if (FMath::Abs(GetCharacterMovement()->Velocity.Z ) > 950.0f && bEnableFallDamage)
	{
		float Damage = (FMath::Abs(GetCharacterMovement()->Velocity.Z ) - 950.0f) / 2.0f;
		UGameplayStatics::ApplyDamage(this, Damage, GetController(), this, FallDamage);
	}
}

void AMultiplayerCharacter::UpdateRun()
{
	
}

void AMultiplayerCharacter::OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal,
	const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta)
{
	Super::OnWalkingOffLedge_Implementation(PreviousFloorImpactNormal, PreviousFloorContactNormal, PreviousLocation,
	                                        TimeDelta);

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StopAllCameraShakes();
}

void AMultiplayerCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StopAllCameraShakes();
}

void AMultiplayerCharacter::OnBeginAiming()
{
	AC_PawnInfo->SetAiming(true);
	MovementType = Aim;
	StartBreathing();
	
	if (AC_InventorySystem->CurrentWeapon)
	{
		bCanRun = false;
	}
}

void AMultiplayerCharacter::OnEndAiming()
{
	StopBreathing();
	AC_PawnInfo->SetAiming(false);
	MovementType = bWantsToRun ? Run : Walk;
	bCanRun = true;
	
}

void AMultiplayerCharacter::AimingTimelineUpdate(float FOVCurve)
{
	if (AC_InventorySystem->CurrentWeapon)
	{
		float FOV = FMath::Lerp(DefaultFOV, AC_InventorySystem->CurrentWeapon->AimingFOV, FOVCurve);
		FollowCamera->SetFieldOfView(FOV);
	}
}

void AMultiplayerCharacter::UpdateWeaponVisibility(bool bShowPrimary)
{
	SetWeaponVisibility(AC_InventorySystem->SecondaryWeapon, bShowPrimary);

	SetWeaponVisibility(AC_InventorySystem->PrimaryWeapon, !bShowPrimary);	
}

void AMultiplayerCharacter::ReloadWeapon()
{
	Server_ReloadWeapon();
}

void AMultiplayerCharacter::ForceUncrouch()
{
	bWantToCrouch = false;
	UnCrouch();
	AC_PawnInfo->SetCrouching(false);
	TryRun();
}

void AMultiplayerCharacter::ForceChangeWeapon(bool Primary, bool QuickChange)
{
	ChangeWeapon(Primary, QuickChange);
}

void AMultiplayerCharacter::ChangeWeapon(bool Primary, bool QuickChange)
{
	if (!AC_PawnInfo->GetUsingLethal())
	{
		bChangingWeapon = true;
		StopShooting();
		
		if (!QuickChange)
		{
			UnequipWeapon();
		}

		ASM_WeaponBase* CurrentWeapon = Primary ? AC_InventorySystem->PrimaryWeapon : AC_InventorySystem->SecondaryWeapon;
		AC_InventorySystem->SetCurrentWeapon(CurrentWeapon);

		AC_InventorySystem->Server_SetCurrentWeapon(AC_InventorySystem->CurrentWeapon);

		EquipWeaponMontage();
	}
}

void AMultiplayerCharacter::UnequipWeapon()
{
}

void AMultiplayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerCharacter, bIsShooting);
	DOREPLIFETIME(AMultiplayerCharacter, HitDirection);
	DOREPLIFETIME(AMultiplayerCharacter, HitBone);
	//DOREPLIFETIME(AMultiplayerCharacter, bIsShooting);
}

void AMultiplayerCharacter::Server_OnPickupWeapon_Implementation(TSubclassOf<ASM_WeaponBase> WeaponClass, int32 Ammo)
{
	AC_InventorySystem->Server_DropWeapon();

	AC_InventorySystem->EquipWeapon(WeaponClass, bIsUsingPrimary(), FName("hand_r"), MeshFP, GetMesh(), Ammo);

	if (bIsUsingPrimary())
	{
		AC_InventorySystem->SetCurrentWeapon(AC_InventorySystem->PrimaryWeapon);
	}
	else
	{
		AC_InventorySystem->SetCurrentWeapon(AC_InventorySystem->SecondaryWeapon);
	}

	UpdateArmsVisibility();
}

void AMultiplayerCharacter::EquipWeaponMontage_Implementation()
{
}

void AMultiplayerCharacter::FPPUnequipWeaponMontage_Implementation()
{
}

void AMultiplayerCharacter::Client_ForceChangeWeapon_Implementation(bool Primary, bool QuickChange)
{
	ForceChangeWeapon(Primary, QuickChange);
}

void AMultiplayerCharacter::PlayMontageReloadWeapon_Implementation(UAnimMontage* MontageToPlay)
{
}

void AMultiplayerCharacter::Multicast_PlayReloadMontage_Implementation()
{
	if (!this->IsLocallyControlled())
	{
		if (AC_InventorySystem->CurrentWeapon)
		{
			PlayMontageReloadWeapon(AC_InventorySystem->CurrentWeapon->AM_TPP_ReloadWeapon);
		}
	}
}

void AMultiplayerCharacter::Server_PlayReloadAnimation_Implementation()
{
	Multicast_PlayReloadMontage();
}

void AMultiplayerCharacter::Server_ReloadWeapon_Implementation()
{
	bool bIsSuccess = false;
	int32 AvailableAmmo = 0;
	AC_InventorySystem->TryReload(bIsSuccess, AvailableAmmo);

	if (bIsSuccess)
	{
		AC_InventorySystem->CurrentWeapon->ReloadWeapon(AvailableAmmo);
		AC_InventorySystem->ReduceCurrentWeaponAmmo(AvailableAmmo);
	}
}

void AMultiplayerCharacter::PlayMontageMeleeWeapon_Implementation(UAnimMontage* MontageToPlay)
{
}

void AMultiplayerCharacter::Multicast_PlayMeleeMontage_Implementation()
{
	if (UGameplayStatics::GetPlayerPawn(GetWorld(), 0) != this)
	{
		if (AC_InventorySystem->CurrentWeapon)
		{
			PlayMontageMeleeWeapon(AC_InventorySystem->CurrentWeapon->AM_TPP_Melee);
		}
	}
}

void AMultiplayerCharacter::Server_PlayTPPMeleeAnimation_Implementation()
{
	Multicast_PlayMeleeMontage();
}

void AMultiplayerCharacter::ClientPlayHitMarkerSound_Implementation()
{
	
}

void AMultiplayerCharacter::Server_ApplyMeleeDamage_Implementation(AActor* DamagedActor,
                                                                   ASM_PlayerControllerBase* CalledInstigator)
{
	UGameplayStatics::ApplyDamage(DamagedActor, 50.0f, this->GetController(), this,
		AC_InventorySystem->CurrentWeapon->MeleeDamageType);

	ClientPlayHitMarkerSound();
}

void AMultiplayerCharacter::AimingTimeline_Implementation()
{
	//BP
}

void AMultiplayerCharacter::PlayHitReactMontage_Implementation()
{
}


void AMultiplayerCharacter::Multicast_PlayHitMontage_Implementation()
{
}

void AMultiplayerCharacter::Client_SetMaxWalkSpeed_Implementation(float MaxWalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AMultiplayerCharacter::StopBreathing_Implementation()
{
}

void AMultiplayerCharacter::StartBreathing_Implementation()
{
}

void AMultiplayerCharacter::PossessedEvent_Implementation()
{
	//BP
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