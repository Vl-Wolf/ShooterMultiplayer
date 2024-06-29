// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Multiplayer/Weapon/SM_BaseLethalEquipment.h"
#include "Multiplayer/Weapon/SM_WeaponBase.h"
#include "SM_AC_InventorySystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquipped, ASM_WeaponBase*, WeaponEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateLethal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFiringModeChanged);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MULTIPLAYER_API USM_AC_InventorySystem : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USM_AC_InventorySystem();
	
	UPROPERTY()
	FOnWeaponEquipped OnWeaponEquipped;

	UPROPERTY()
	FOnUpdateAmmo OnUpdateAmmo;

	UPROPERTY()
	FOnUpdateLethal OnUpdateLethal;

	UPROPERTY()
	FOnFiringModeChanged OnFiringModeChanged;
	
	UPROPERTY(ReplicatedUsing=OnRep_PrimaryWeapon)
	ASM_WeaponBase* PrimaryWeapon = nullptr;
	
	UPROPERTY(ReplicatedUsing=OnRep_SecondaryWeapon)
	ASM_WeaponBase* SecondaryWeapon = nullptr;

public:
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeapon)
	ASM_WeaponBase* CurrentWeapon = nullptr;
	
	UPROPERTY()
	ASM_WeaponBase* PreviousWeapon = nullptr;

	UPROPERTY(Replicated)
	ASM_BaseLethalEquipment* LethalEquipment = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_WeaponAmmo)
	TArray<FWeaponAmmo> WeaponAmmo;

	UPROPERTY(ReplicatedUsing=OnRep_LethalAmmo)
	TArray<FLethalAmmo> LethalAmmo;

public:
	
	UFUNCTION()
	virtual void OnRep_PrimaryWeapon();

	UFUNCTION()
	virtual void OnRep_SecondaryWeapon();

	UFUNCTION()
	virtual void OnRep_CurrentWeapon();

	UFUNCTION()
	virtual void OnRep_WeaponAmmo();

	UFUNCTION()
	virtual void OnRep_LethalAmmo();

	UFUNCTION()
	void CreateAndEquipLoadout(TSubclassOf<ASM_WeaponBase> PrimaryWeaponToSpawn,
		TSubclassOf<ASM_WeaponBase> SecondaryWeaponToSpawn, TSubclassOf<ASM_BaseLethalEquipment> LethalEquipmentToSpawn,
		USkeletalMeshComponent* FPP_Mesh, USkeletalMeshComponent* TPP_Mesh);

	UFUNCTION()
	void AttachWeapon(ASM_WeaponBase* WeaponToAttach, USkeletalMeshComponent* FPP_Mesh, USkeletalMeshComponent* TPP_Mesh);

	UFUNCTION()
	void SetCurrentWeapon(ASM_WeaponBase* NewCurrentWeapon);

	UFUNCTION()
	void EquipWeapon(TSubclassOf<ASM_WeaponBase> Class, bool bIsPrimary, FName Socket, USkeletalMeshComponent* FPP_Mesh,
		USkeletalMeshComponent* TPP_Mesh, int32 Ammo);

	UFUNCTION()
	void AddAmmoToCurrentWeapon();

	UFUNCTION()
	FWeaponAmmo GetAmmoInfoForCurrentWeapon();

	UFUNCTION()
	void Refillammo();

	UFUNCTION()
	void TryReload(bool& bSuccess, int32 AvailableAmmo);

	UFUNCTION()
	int32 GetAmmoNeededToReload();

	UFUNCTION()
	EAmmoType GetCurrentWeaponAmmoType();

	UFUNCTION()
	void ReduceCurrentWeaponAmmo(int32 AmmoToReduce);

	UFUNCTION()
	ASM_WeaponBase* GetCurrentWeapon();

	UFUNCTION()
	FWeaponAmmo GetAmmoPertype(EAmmoType AmmoType);

	UFUNCTION()
	bool IsFullAmmo(EAmmoType AmmoType);

	UFUNCTION()
	void AddLethalAmmo();

	UFUNCTION()
	void ReduceLethalAmmo();

	UFUNCTION()
	bool IsFullLethalAmmo();

	UFUNCTION()
	bool CheckLethalAmmo();

	UFUNCTION()
	ASM_BaseLethalEquipment* GetLethalEquipment();

	UFUNCTION()
	int32 GetLethalAmmo();

	UFUNCTION()
	EEquippedWeapon GetCurrentWeaponType();

	UFUNCTION()
	bool HasAmmoForCurrentWeapon();

	UFUNCTION()
	bool CheckCurrentWeaponAmmo();

	UFUNCTION(Server, Reliable)
	void Server_CreateAndEquipLoadout(TSubclassOf<ASM_WeaponBase> PrimaryWeaponToSpawn,
		TSubclassOf<ASM_WeaponBase> SecondaryWeaponToSpawn, TSubclassOf<ASM_BaseLethalEquipment> LethalEquipmentToSpawn,
		USkeletalMeshComponent* FPP_Mesh, USkeletalMeshComponent* TPP_Mesh);

	UFUNCTION(Server, Reliable)
	void Server_DropWeapon();

	UFUNCTION(Server, Reliable)
	void Server_SetCurrentWeapon(ASM_WeaponBase* NewCurrentWeapon);

	UFUNCTION()
	void DestroyWeapons();

private:
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
		
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
