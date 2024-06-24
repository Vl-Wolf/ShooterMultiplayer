// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PawnActions.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPawnActions : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MULTIPLAYER_API IPawnActions
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION()
	virtual void OnPickupWeapon(UClass* WeaponClass, int32 CurrentAmmo);
	
	UFUNCTION()
	virtual void FireWeapon();
	
	UFUNCTION()
	virtual  void StopFiringWeapon();
	
	UFUNCTION()
	virtual void PawnTryReload();

	UFUNCTION()
	virtual void HideFPPMesh();

	UFUNCTION()
	virtual void ShowFPPMesh();

	UFUNCTION()
	virtual void HideTPPMesh();

	UFUNCTION()
	virtual void ShowTPPMesh();

	UFUNCTION()
	virtual void ShowLethalEquipment();

	UFUNCTION()
	virtual void HideLethalEquipment();

	UFUNCTION()
	virtual void PlayFPPEquipWeapon();

	UFUNCTION()
	virtual void PlayTPPEquipWeapon();

	UFUNCTION()
	virtual void HideWeapon();

	UFUNCTION()
	virtual void GetPawnMeshes(USkeletalMeshComponent* FPPMesh, USkeletalMeshComponent* TPPMesh);

	UFUNCTION()
	virtual void TryDropItems();

};
