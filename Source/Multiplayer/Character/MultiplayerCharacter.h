// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Multiplayer/ActorComponents/SM_AC_Health.h"
#include "Multiplayer/ActorComponents/SM_AC_InventorySystem.h"
#include "Multiplayer/ActorComponents/SM_AC_PawnInfo.h"
#include "MultiplayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartFiring);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopFiring);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);
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
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

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
	void HealPlayer(float HealAmount, AController* HealInstigator);
	
	float HealAmount = 0.0f;

	UFUNCTION()
	void DamagePlayer(float Damage, UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	float TakeDamage(float DamageAmount, AController* ControllerInstigator);

	void HandleDeath(AActor* Dead_Actor, AController* InstigatorController, UDamageType* DmgType, AActor* DamageCauser);

	UFUNCTION()
	void ShowWeapon();
};

