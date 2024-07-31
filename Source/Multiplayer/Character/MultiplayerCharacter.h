// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Multiplayer/ActorComponents/SM_AC_Health.h"
#include "Multiplayer/ActorComponents/SM_AC_InventorySystem.h"
#include "Multiplayer/ActorComponents/SM_AC_PawnInfo.h"
#include "Multiplayer/PlayerController/SM_PlayerControllerBase.h"
#include "MultiplayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartFiring);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopFiring);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPerspectiveChange);

UCLASS(config=Game)
class AMultiplayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess))
	USkeletalMeshComponent* MeshFP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess))
	USM_AC_Health* AC_Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess))
	USM_AC_InventorySystem* AC_InventorySystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess))
	USM_AC_PawnInfo* AC_PawnInfo;

public:
	
	AMultiplayerCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	UPROPERTY()
	FOnPerspectiveChange OnPerspectiveChange;

	//UPROPERTY()
	//FOnPlayerDead OnPlayerDead;

	UPROPERTY()
	FOnStartFiring OnStartFiring;

	UPROPERTY()
	FOnStopFiring OnStopFiring;

	
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:

	//UPROPERTY()
	FVector SpringArmInitialLocation = FVector::Zero();

	float InterpSpeed = 0.0f;

	float PitchAcum = 0.0f;
	float YawAcum = 0.0f;

	UFUNCTION()
	void AddRecoil();

	UFUNCTION()
	void SetPOV();

	float CurrentFOV = 90.0f;
	float TargetFOV = 0.0f;

	UFUNCTION()
	float MovementRecoil(ASM_WeaponBase* Weapon);

	bool bIsFirstShot = false;

	UFUNCTION()
	void SetWeaponVisibility(ASM_WeaponBase* Weapon, bool Visible);

	UFUNCTION()
	void SetShooting(bool bShooting);
	
	UFUNCTION()
	void StopShooting();

	FTimerHandle AutoShootTimerHandle;

	UFUNCTION()
	void TryRun();

	UFUNCTION()
	void AddRecoilPitch();

	float ActualPitch = 0.0f;
	
	UFUNCTION()
	void ResetRecoil();

	UFUNCTION()
	void AddRecoilYaw();

	float ActualYaw = 0.0f;
	
	UFUNCTION()
	void AdjustCamera(float Input);

	UFUNCTION()
	void OnDamageTaken(float Damage, UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	UFUNCTION()
	void MeleeAttack();

	float MeleeAttackRange = 150.0f;

	UFUNCTION()
	void OnDead(AActor* DeadActor, AController* InstigatorController, UDamageType* DamageType, AActor* DamageCauser);
	
	UPROPERTY()
	bool bIsWantsToRun = false;

	float DefaultFOV = 90.0f;

	UFUNCTION()
	void OnTurn(float Input);

	UFUNCTION()
	void OnLookUp(float Input);

	float ReduceAimingSensitivity(float Input, float MouseSensitivity, float AimSensitivity);

	UFUNCTION()
	void OnMoveForward(float Input);

	float InputValue = 0.0f;
	
	UFUNCTION()
	void OnMoveRight(float Input);

	UFUNCTION()
	void UpdatePerspective();

	UFUNCTION()
	void ChangeFiringMode();

	bool bIsCanShoot = false;
	bool bIsFiringBurst = false;
	bool bIsPerformingAction = false;
	bool bIsChangingWeapon = false;
	bool bIsFireEnabled = false;

	UFUNCTION()
	void PawnDeath();

	UFUNCTION()
	void UpdateArmsVisibility();

	UFUNCTION()
	void UpdateSpringArmPosition();

	float TPPSpringArmLength = 150.0f;

	UFUNCTION()
	void ResetCollision();

	UFUNCTION()
	void HealPlayer(float NeedHealAmount, AController* HealInstigator);
	
	float HealAmount = 0.0f;

	UFUNCTION()
	void DamagePlayer(float Damage, UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void HandleDeath(AActor* Dead_Actor, AController* InstigatorController, UDamageType* DmgType, AActor* DamageCauser);

	UFUNCTION()
	void ShowWeapon();

	UFUNCTION()
	void AttachWeapon(ASM_WeaponBase* WeaponCreated);

	UFUNCTION()
	void SimulatedPhysicsOnWeapon(ASM_WeaponBase* Weapon, bool bIsSimulate);

	UFUNCTION()
	void StopCurrentWeaponReload();

	UFUNCTION()
	bool bIsUsingPrimary();

	UPROPERTY(ReplicatedUsing=OnRep_bIsShooting)
	bool bIsShooting = false;

	UFUNCTION()
	void OnRep_bIsShooting();

	UFUNCTION()
	float CalculateMovementInput(float Input);

	ECharacterMovement MovementType = Walk;

	UFUNCTION()
	float ScaleAxisInput(float InputAxisValue);

	UFUNCTION()
	float GetMultiplier();

	UFUNCTION()
	void Ragdoll();

	UPROPERTY(Replicated)
	FVector HitDirection = FVector::Zero();

	UPROPERTY(Replicated)
	FName HitBone;
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TArray<TSoftClassPtr<ASM_WeaponBase>> Weapons;

private:

	UFUNCTION()
	TSoftClassPtr<ASM_WeaponBase> GetRandomWeaponToSpawn();

	UFUNCTION()
	void GetMovementSpeed();

	UFUNCTION()
	void AITryRun();

	UFUNCTION()
	void SetWeaponAnimBlueprints(ASM_WeaponBase* Weapon);

	UFUNCTION()
	void EnableDamage();

	virtual void PossessedBy(AController* NewController) override;

public:
	
	UFUNCTION(BlueprintNativeEvent)
	void PossessedEvent();

private:

	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION()
	void UpdateRun();

	bool bEnableFallDamage = true;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category= "Damage")
	TSubclassOf<UDamageType> FallDamage = nullptr;

private:

	virtual void OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal,
		const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta) override;

	virtual void OnJumped_Implementation() override;

public:
	
	UFUNCTION(BlueprintNativeEvent)
	void StartBreathing();

	UFUNCTION(BlueprintNativeEvent)
	void StopBreathing();

private:
	
	bool bEnableBreathing = true;

	/*UPROPERTY()
	UTimelineComponent* BreathingTimeline = nullptr;*/

	UFUNCTION(Client, Reliable)
	void Client_SetMaxWalkSpeed(float MaxWalkSpeed);

	UFUNCTION(NetMulticast, UnReliable)
	void Multicast_PlayHitMontage();

public:
	
	UFUNCTION(BlueprintNativeEvent)
	void PlayHitReactMontage();

private:

	UFUNCTION()
	void OnBeginAiming();

	bool bCanRun = true;
	bool bWantsToRun = false;
	
	UFUNCTION()
	void OnEndAiming();

public:

	UFUNCTION(BlueprintNativeEvent)
	void AimingTimeline();

	UFUNCTION(BlueprintCallable)
	void AimingTimelineUpdate(float FOVCurve);

private:

	UFUNCTION()
	void UpdateWeaponVisibility(bool bShowPrimary);

	UFUNCTION(Server, Reliable)
	void Server_ApplyMeleeDamage(AActor* DamagedActor, ASM_PlayerControllerBase* CalledInstigator);

	UFUNCTION(Client, UnReliable)
	void ClientPlayHitMarkerSound();

	UFUNCTION(Server, Reliable)
	void Server_PlayTPPMeleeAnimation();

	UFUNCTION(NetMulticast, UnReliable)
	void Multicast_PlayMeleeMontage();

public:

	UFUNCTION(BlueprintNativeEvent)
	void PlayMontageMeleeWeapon(UAnimMontage* MontageToPlay);

private:

	UFUNCTION()
	void ReloadWeapon();

	UFUNCTION(Server, Reliable)
	void Server_ReloadWeapon();

	UFUNCTION(Server, Reliable)
	void Server_PlayReloadAnimation();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayReloadMontage();

public:

	UFUNCTION(BlueprintNativeEvent)
	void PlayMontageReloadWeapon(UAnimMontage* MontageToPlay);

private:

	UFUNCTION()
	void ForceUncrouch();

	bool bWantToCrouch = false;

	UFUNCTION(Client, Unreliable)
	void Client_ForceChangeWeapon(bool Primary, bool QuickChange);

	UFUNCTION()
	void ForceChangeWeapon(bool Primary, bool QuickChange);

	UFUNCTION(BlueprintCallable)
	void ChangeWeapon(bool Primary, bool QuickChange);

	bool bChangingWeapon = false;

	UFUNCTION()
	void UnequipWeapon();

public:
	
	UFUNCTION(BlueprintNativeEvent)
	void FPPUnequipWeaponMontage();

	UFUNCTION(BlueprintNativeEvent)
	void EquipWeaponMontage();

private:

	UFUNCTION(Server, Reliable)
	void Server_OnPickupWeapon(TSubclassOf<ASM_WeaponBase> WeaponClass, int32 Ammo);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};

