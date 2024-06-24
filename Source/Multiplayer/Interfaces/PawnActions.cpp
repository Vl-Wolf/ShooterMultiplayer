// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnActions.h"


// Add default functionality here for any IPawnActions functions that are not pure virtual.
void IPawnActions::OnPickupWeapon(UClass* WeaponClass, int32 CurrentAmmo)
{
}

void IPawnActions::FireWeapon()
{
}

void IPawnActions::StopFiringWeapon()
{
}

void IPawnActions::PawnTryReload()
{
}

void IPawnActions::HideFPPMesh()
{
}

void IPawnActions::ShowFPPMesh()
{
}

void IPawnActions::HideTPPMesh()
{
}

void IPawnActions::ShowTPPMesh()
{
}

void IPawnActions::ShowLethalEquipment()
{
}

void IPawnActions::HideLethalEquipment()
{
}

void IPawnActions::PlayFPPEquipWeapon()
{
}

void IPawnActions::PlayTPPEquipWeapon()
{
}

void IPawnActions::HideWeapon()
{
}

void IPawnActions::GetPawnMeshes(USkeletalMeshComponent* FPPMesh, USkeletalMeshComponent* TPPMesh)
{
}

void IPawnActions::TryDropItems()
{
}
