// Fill out your copyright notice in the Description page of Project Settings.


#include "SM_AC_InventorySystem.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Multiplayer/Interfaces/PawnActions.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USM_AC_InventorySystem::USM_AC_InventorySystem()
{
	PrimaryComponentTick.bCanEverTick = true;

	/*WeaponAmmo[0].AmmoType = AssaultRifleType;
	WeaponAmmo[0].Count = 90;
	WeaponAmmo[0].MaxCount = 120;

	WeaponAmmo[1].AmmoType = ShotgunType;
	WeaponAmmo[1].Count = 18;
	WeaponAmmo[1].MaxCount = 36;

	WeaponAmmo[2].AmmoType = SniperRifleType;
	WeaponAmmo[2].Count = 10;
	WeaponAmmo[2].MaxCount = 25;

	WeaponAmmo[3].AmmoType = PistolType;
	WeaponAmmo[3].Count = 30;
	WeaponAmmo[3].MaxCount = 90;

	WeaponAmmo[4].AmmoType = NoneType;
	WeaponAmmo[4].Count = 0;
	WeaponAmmo[4].MaxCount = 0;

	LethalAmmo[0].LethalAmmoType = Grenade;
	LethalAmmo[0].Count = 2;
	LethalAmmo[0].MaxCount = 2;

	LethalAmmo[1].LethalAmmoType = Claymore;
	LethalAmmo[1].Count = 1;
	LethalAmmo[1].MaxCount = 2;*/
}


void USM_AC_InventorySystem::OnRep_PrimaryWeapon()
{
	USkeletalMeshComponent* FPP_Mesh = nullptr;
	USkeletalMeshComponent* TPP_Mesh = nullptr;

	if (IPawnActions* PawnActions = Cast<IPawnActions>(GetOwner()))
	{
		PawnActions->GetPawnMeshes(FPP_Mesh, TPP_Mesh);
	}
	
	AttachWeapon(PrimaryWeapon, FPP_Mesh, TPP_Mesh);

	PrimaryWeapon->MeshFPP->SetVisibility(PrimaryWeapon == CurrentWeapon, true);
	PrimaryWeapon->MeshTPP->SetVisibility(PrimaryWeapon == CurrentWeapon, true);
}

void USM_AC_InventorySystem::OnRep_SecondaryWeapon()
{
	USkeletalMeshComponent* FPP_Mesh = nullptr;
	USkeletalMeshComponent* TPP_Mesh = nullptr;
	
	if (IPawnActions* PawnActions = Cast<IPawnActions>(GetOwner()))
	{
		PawnActions->GetPawnMeshes(FPP_Mesh, TPP_Mesh);
	}

	AttachWeapon(SecondaryWeapon, FPP_Mesh, TPP_Mesh);

	SecondaryWeapon->MeshFPP->SetVisibility(SecondaryWeapon == CurrentWeapon, true);
	SecondaryWeapon->MeshTPP->SetVisibility(SecondaryWeapon == CurrentWeapon, true);
}

void USM_AC_InventorySystem::OnRep_CurrentWeapon()
{
	OnWeaponEquipped.Broadcast(CurrentWeapon);

	if (UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		if (GetOwner() == UGameplayStatics::GetPlayerPawn(GetWorld(), 0) && UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->IsLocallyControlled())
		{
			// UI
		}
		else
		{
			if (CurrentWeapon)
			{
				CurrentWeapon->MeshTPP->SetVisibility(true, false);
			}

			if (PreviousWeapon)
			{
				PreviousWeapon->MeshTPP->SetVisibility(false, false);
			}
		}
	}

	PreviousWeapon = CurrentWeapon;
}

void USM_AC_InventorySystem::OnRep_WeaponAmmo()
{
	OnUpdateAmmo.Broadcast();
}

void USM_AC_InventorySystem::OnRep_LethalAmmo()
{
	OnUpdateLethal.Broadcast();
}

void USM_AC_InventorySystem::CreateAndEquipLoadout(TSubclassOf<ASM_WeaponBase> PrimaryWeaponToSpawn,
	TSubclassOf<ASM_WeaponBase> SecondaryWeaponToSpawn, TSubclassOf<ASM_BaseLethalEquipment> LethalEquipmentToSpawn,
	USkeletalMeshComponent* FPP_Mesh, USkeletalMeshComponent* TPP_Mesh)
{
}

void USM_AC_InventorySystem::AttachWeapon(ASM_WeaponBase* WeaponToAttach, USkeletalMeshComponent* FPP_Mesh,
	USkeletalMeshComponent* TPP_Mesh)
{
	FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	AttachmentTransformRules.LocationRule = EAttachmentRule::KeepRelative;
	AttachmentTransformRules.RotationRule = EAttachmentRule::KeepRelative;
	AttachmentTransformRules.ScaleRule = EAttachmentRule::KeepRelative;

	WeaponToAttach->MeshFPP->AttachToComponent(FPP_Mesh, AttachmentTransformRules, FName("hand_r"));
	WeaponToAttach->MeshTPP->AttachToComponent(TPP_Mesh, AttachmentTransformRules, FName("hand_r"));	
}

void USM_AC_InventorySystem::SetCurrentWeapon(ASM_WeaponBase* NewCurrentWeapon)
{
	this->CurrentWeapon = NewCurrentWeapon;
}

void USM_AC_InventorySystem::EquipWeapon(TSubclassOf<ASM_WeaponBase> Class, bool bIsPrimary, FName Socket,
	USkeletalMeshComponent* FPP_Mesh, USkeletalMeshComponent* TPP_Mesh, int32 Ammo)
{
	if (Class)
	{
		APawn* Pawn = Cast<APawn>(GetOwner());
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = GetOwner();
		ActorSpawnParameters.Instigator = Pawn;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;
		ActorSpawnParameters.TransformScaleMethod = ESpawnActorScaleMethod::SelectDefaultAtRuntime;
				
		ASM_WeaponBase* WeaponBase = GetWorld()->SpawnActor<ASM_WeaponBase>(Class, GetOwner()->GetActorTransform(), ActorSpawnParameters);

		WeaponBase->PickupWeaponAmmo = Ammo;
		WeaponBase->InitialValues = true;

		if (bIsPrimary)
		{
			PrimaryWeapon = WeaponBase;
			PrimaryWeapon->SetOwner(GetOwner());
			AttachWeapon(PrimaryWeapon, FPP_Mesh, TPP_Mesh);
		}
		else
		{
			SecondaryWeapon = WeaponBase;
			SecondaryWeapon->SetOwner(GetOwner());
			AttachWeapon(SecondaryWeapon, FPP_Mesh, TPP_Mesh);
		}
		
		
	}
}

void USM_AC_InventorySystem::AddAmmoToCurrentWeapon()
{
	for (int32 i = 0; i < WeaponAmmo.Num() - 1; i++)
	{
		if (WeaponAmmo[i].AmmoType == GetCurrentWeaponAmmoType())
		{
			if (WeaponAmmo[i].Count < WeaponAmmo[i].MaxCount)
			{
				//UKismetArrayLibrary::Array_Set(WeaponAmmo, i, )

				WeaponAmmo[i].AmmoType = WeaponAmmo[i].AmmoType;
				WeaponAmmo[i].Count = FMath::Clamp(WeaponAmmo[i].Count + GetCurrentWeapon()->AmmoPerMag, 0, WeaponAmmo[i].MaxCount);
				WeaponAmmo[i].MaxCount = WeaponAmmo[i].MaxCount;
			}
		}
	}

	OnUpdateAmmo.Broadcast();
}

FWeaponAmmo USM_AC_InventorySystem::GetAmmoInfoForCurrentWeapon()
{
	FWeaponAmmo CurrentWeaponAmmo_NULL;
	
	for (FWeaponAmmo CurrentWeaponAmmo : WeaponAmmo)
	{
		if (GetCurrentWeaponAmmoType() == CurrentWeaponAmmo.AmmoType)
		{
			return CurrentWeaponAmmo;
		}
	}

	return CurrentWeaponAmmo_NULL;
}

void USM_AC_InventorySystem::Refillammo()
{
	for (int32 i = 0; i < WeaponAmmo.Num() - 1; i++)
	{
		if (WeaponAmmo[i].Count < WeaponAmmo[i].MaxCount)
		{
			WeaponAmmo[i].AmmoType = WeaponAmmo[i].AmmoType;
			WeaponAmmo[i].Count = WeaponAmmo[i].MaxCount;
			WeaponAmmo[i].MaxCount = WeaponAmmo[i].MaxCount;
		}
	}

	OnUpdateAmmo.Broadcast();
}

void USM_AC_InventorySystem::TryReload(bool& bSuccess, int32 AvailableAmmo)
{
	for (int32 i = 0; i < WeaponAmmo.Num() - 1; i++)
	{
		if (WeaponAmmo[i].AmmoType == GetCurrentWeaponAmmoType() && WeaponAmmo[i].Count >= 1)
		{
			bSuccess = true;
			AvailableAmmo = FMath::Min(WeaponAmmo[i].Count, GetAmmoNeededToReload());
		}
	}

	AvailableAmmo = 0;
	bSuccess = false;
	
}

int32 USM_AC_InventorySystem::GetAmmoNeededToReload()
{
	return CurrentWeapon->AmmoPerMag - CurrentWeapon->ActualAmmo;
}

EAmmoType USM_AC_InventorySystem::GetCurrentWeaponAmmoType()
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->AmmoType;
	}

	return NoneType;
}

void USM_AC_InventorySystem::ReduceCurrentWeaponAmmo(int32 AmmoToReduce)
{
	for (int32 i = 0; i < WeaponAmmo.Num() - 1; i++)
	{
		if (WeaponAmmo[i].AmmoType == GetCurrentWeaponAmmoType())
		{
			WeaponAmmo[i].AmmoType = WeaponAmmo[i].AmmoType;
			WeaponAmmo[i].Count = WeaponAmmo[i].Count - AmmoToReduce;
			WeaponAmmo[i].MaxCount = WeaponAmmo[i].MaxCount;
		}
	}

	OnUpdateAmmo.Broadcast();
}

ASM_WeaponBase* USM_AC_InventorySystem::GetCurrentWeapon()
{
	return CurrentWeapon;
}

FWeaponAmmo USM_AC_InventorySystem::GetAmmoPertype(EAmmoType AmmoType)
{
	FWeaponAmmo CurrentWeaponAmmo_NULL;
	
	for (FWeaponAmmo CurrentWeaponAmmo : WeaponAmmo)
	{
		if (CurrentWeaponAmmo.AmmoType == AmmoType)
		{
			return CurrentWeaponAmmo;
		}
	}

	return CurrentWeaponAmmo_NULL;
}

bool USM_AC_InventorySystem::IsFullAmmo(EAmmoType AmmoType)
{
	for (int32 i = 0; i < WeaponAmmo.Num() - 1; i++)
	{
		if (WeaponAmmo[i].AmmoType == AmmoType)
		{
			if (WeaponAmmo[i].Count == WeaponAmmo[i].MaxCount)
			{
				return true;
			}
		}
	}

	return false;
}

void USM_AC_InventorySystem::AddLethalAmmo()
{
	for (int32 i = 0; i < LethalAmmo.Num() - 1; i++)
	{
		/*if (WeaponAmmo[i].AmmoType == LethalEquipment-)
		{
			
		}*/
	}

	OnUpdateAmmo.Broadcast();
}

void USM_AC_InventorySystem::ReduceLethalAmmo()
{
}

bool USM_AC_InventorySystem::IsFullLethalAmmo()
{
	return false;
}

bool USM_AC_InventorySystem::CheckLethalAmmo()
{
	return false;
}

ASM_BaseLethalEquipment* USM_AC_InventorySystem::GetLethalEquipment()
{
	return LethalEquipment;
}

int32 USM_AC_InventorySystem::GetLethalAmmo()
{
	return 0;
}

EEquippedWeapon USM_AC_InventorySystem::GetCurrentWeaponType()
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->WeaponType;
	}

	return None;
}

bool USM_AC_InventorySystem::HasAmmoForCurrentWeapon()
{
	for (int32 i = 0; i < WeaponAmmo.Num() - 1; i++)
	{
		if (WeaponAmmo[i].AmmoType == GetCurrentWeaponAmmoType())
		{
			if (WeaponAmmo[i].Count > 0)
			{
				return true;
			}
		}
	}

	return false;
}

bool USM_AC_InventorySystem::CheckCurrentWeaponAmmo()
{
	if (CurrentWeapon)
	{
		if (CurrentWeapon->ActualAmmo == 0)
		{
			return true;
		}
	}

	return false;
}

void USM_AC_InventorySystem::Server_CreateAndEquipLoadout_Implementation(
	TSubclassOf<ASM_WeaponBase> PrimaryWeaponToSpawn, TSubclassOf<ASM_WeaponBase> SecondaryWeaponToSpawn,
	TSubclassOf<ASM_BaseLethalEquipment> LethalEquipmentToSpawn, USkeletalMeshComponent* FPP_Mesh,
	USkeletalMeshComponent* TPP_Mesh)
{
	CreateAndEquipLoadout(PrimaryWeaponToSpawn, SecondaryWeaponToSpawn, LethalEquipmentToSpawn, FPP_Mesh, TPP_Mesh);
}

void USM_AC_InventorySystem::Server_DropWeapon_Implementation()
{
	
}

void USM_AC_InventorySystem::Server_SetCurrentWeapon_Implementation(ASM_WeaponBase* NewCurrentWeapon)
{
	SetCurrentWeapon(NewCurrentWeapon);
}

void USM_AC_InventorySystem::DestroyWeapons()
{
	if (PrimaryWeapon)
	{
		PrimaryWeapon->Destroy();

		if (SecondaryWeapon)
		{
			SecondaryWeapon->Destroy();
		}
	}
	else
	{
		SecondaryWeapon->Destroy();
	}
}

void USM_AC_InventorySystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	DestroyWeapons();
}

void USM_AC_InventorySystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USM_AC_InventorySystem, PrimaryWeapon);
	DOREPLIFETIME(USM_AC_InventorySystem, SecondaryWeapon);
	DOREPLIFETIME(USM_AC_InventorySystem, CurrentWeapon);
	DOREPLIFETIME(USM_AC_InventorySystem, LethalEquipment);
	DOREPLIFETIME_CONDITION(USM_AC_InventorySystem, WeaponAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(USM_AC_InventorySystem, LethalAmmo, COND_OwnerOnly);
}


