// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Multiplayer/EnumsAndStructs/SM_Types.h"
#include "Sound/SoundCue.h"
#include "SM_WeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmmoChanged);

UCLASS()
class MULTIPLAYER_API ASM_WeaponBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASM_WeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Deafult")
	USceneComponent* SceneComponent = nullptr;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Deafult")
	USkeletalMeshComponent* MeshFPP = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Deafult")
	USkeletalMeshComponent* MeshTPP = nullptr;

protected:
	
	UPROPERTY()
	FOnAmmoChanged OnAmmoChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Effects")
	UParticleSystem* MuzzleFlashEmitter = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Effects")
	UParticleSystem* ImpactDefaultEmitter = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Effects")
	UParticleSystem* ImpactBloodEmitter = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Effects")
	UMaterialInterface* ImpactDecal = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | FPP")
	UAnimMontage* AM_FPP_EquipWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | FPP")
	UAnimMontage* AM_FPP_UnequipWeapon = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | FPP")
	UAnimMontage* AM_FPP_FireWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | FPP")
	UAnimMontage* AM_FPP_ReloadWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | FPP")
	UAnimMontage* AM_FPP_FireInsightWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | FPP")
	UAnimMontage* AM_FPP_Melee = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | TPP")
	UAnimMontage* AM_TPP_EquipWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | TPP")
	UAnimMontage* AM_TPP_UnequipWeapon = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | TPP")
	UAnimMontage* AM_TPP_FireWeaponLoop = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | TPP")
	UAnimMontage* AM_TPP_ReloadWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | TPP")
	UAnimMontage* AM_TPP_FireWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | TPP")
	UAnimMontage* AM_TPP_Melee = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | Weapon")
	UAnimSequence* ShootAnimation = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Animations | Weapon")
	UAnimSequence* ReloadWeaponAnimation = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Camera Shake")
	TSubclassOf<UCameraShakeBase> FireShake = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Camera Shake")
	TSubclassOf<UCameraShakeBase> BreathingShake = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Sound")
	USoundCue* NoAmmoFireSound = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Sound")
	USoundBase* FireSound = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Sound")
	USoundBase* ImpactDefaultSound = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Sound")
	TSubclassOf<UCameraShakeBase> ImpactFleshSound = nullptr;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	int32 AmmoPerMag = 30;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_ActualAmmo, Category="Settings")
	int32 ActualAmmo = 5;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float Damage = 50;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float FireRate = 0.1f;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float WeaponRange = 75000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float WeaponSpread = 0.02f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float WeaponVerticalRecoilMax = 0.4f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float WeaponVerticalRecoilMin = 0.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float WeaponHorizontalRecoilMax = 0.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float WeaponHorizontalRecoilMin = 0.05f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float AimingSpread = 0.09f;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	TEnumAsByte<EAmmoType> AmmoType = EAmmoType::AssaultRifleType;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category="Settings")
	int32 PickupWeaponAmmo = 5;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category="Settings")
	bool InitialValues = true;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	bool bHitScanBullets = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	bool bIsShotgun = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	int32 ShotgunProjectilesPerShot = 6;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	TSubclassOf<UDamageType> MeleeDamageType = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category="Settings")
	TSubclassOf<UDamageType> DamageType = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	TArray<TEnumAsByte<EFiringMode>> FiringModes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	TEnumAsByte<EFiringMode> ActualFiringMode = EFiringMode::AUTO;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	int32 BulletsPerBurst = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float AimingFOV = 60.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float AimingInterSpeed = 15.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	TEnumAsByte<EEquippedWeapon> WeaponName = EEquippedWeapon::AR;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float HeadshotMultiplier = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float BulletVelocity = 60000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float RecoverInterpSpeed = 12.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float RecoilInterpSpeed = 6.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float BurstFireRate = 0.07f;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	TEnumAsByte<EEquippedWeapon> WeaponType = EEquippedWeapon::None;

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	FTransform FPP_WeaponOffsetTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	FTransform TPP_WeaponOffsetTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	bool bShouldHideCrossHairTPP = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float NoiseRange = 3500.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Settings")
	float NoiseLoudness = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bHasSuppressor = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bUseClientHitReg = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UAnimInstance* FPP_WeaponAnims = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UAnimInstance* TPP_WeaponAnims = nullptr;

public:
	
	UFUNCTION()
	void ReduceAmmo();

	UFUNCTION()
	void SpawnBullet(FVector SpawnLocation, FVector Velocity, APawn* AInstigator, FRotator SpawnRotation, float Spread, bool bIsClientSideOnly);

	UFUNCTION()
	bool CheckBulletPath();

	UFUNCTION()
	void FireInstant(FVector TraceStart, FVector ForwardVector, APawn* AAInstigator, float Spread, bool bIsClientSideOnly);

	UFUNCTION()
	void GetLineTraceInfo(FVector& TraceStart, FVector& ForwardVector);

	UFUNCTION()
	void GetSpawnBulletInfo(FVector& SpawnLocation, FVector& AVelocity, FRotator& SpawnRotation);

	UFUNCTION()
	void OnInstantBulletHit(FHitResult& Hit, float DmgMultiplier, APawn* AAAInstigator);
		
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayEffects(FHitResult Hit);
	
	UFUNCTION()
	void CreateCompassMarker();

	UFUNCTION()
	virtual void OnRep_ActualAmmo();

	UFUNCTION()
	void ReloadWeapon(int32 AddedAmmo);

	UFUNCTION()
	void ChangeFiringMode();

	UFUNCTION()
	void UpdateWeaponVisibility(bool bIsThirdPerson);

	UFUNCTION(Server, Reliable)
	void Server_SetActualAmmo(int32 NewActualAmmo);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayShotEffects();

	UFUNCTION()
	void LocalPlayEffects();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayImpactEffects(const FHitResult& HitResult);
	
	UFUNCTION(Server, Reliable)
	void Server_PlayImpactEffects(const FHitResult& HitResult);
	
	UFUNCTION(Server, Reliable)
	void Server_PlayShootEffects();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayMultipleImpactEffects(const TArray<FHitResult>& HitResults);

	UFUNCTION(Server, Reliable)
	void Server_PlayHitImpactEffect(const TArray<FHitResult>& HitResults);

	UFUNCTION()
	void PlayShootShake();

	UFUNCTION()
	void PlayWeaponFireSound();

	UFUNCTION()
	void PlayWeaponShotAnimation();

	UFUNCTION()
	void Shoot(float Spread);

	UFUNCTION(Server, Reliable)
	void Server_FireBullet(FVector SpawnLocation, FVector Velocity, APawn* AInstigator, FRotator SpawnRotation, float Spread);

	UFUNCTION(Server, Reliable)
	void Server_FireInstantBullet(FVector TraceStart, FVector ForwardVector, APawn* AInstigator, float Spread);

	UFUNCTION(Server, Reliable)
	void Server_InstantBulletHit(FHitResult Hit, float DmgMultiplier, APawn* AInstigator);

	UFUNCTION()
	void ClientSideHitReg(float Spread);
	
	UFUNCTION()
	void ServerSideHitReg(float Spread);

	UFUNCTION(Server, Reliable)
	void Server_BulletApplyDamage(const int32 ActorsHitted, APawn* InstigatorCharacter, const FHitResult& HitInfo);

	UFUNCTION(Server, Reliable)
	void Server_ReduceAmmo();

	//Remove

	UPROPERTY()
	bool bIsTPP = false;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
