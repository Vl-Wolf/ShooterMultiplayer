// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SM_Types.generated.h"

UENUM(BlueprintType)
enum ECharacterMovement
{
	Walk UMETA(DisplayName = "Walk"),
	Run UMETA(DisplayName = "Run"),
	Aim UMETA(DisplayName = "Aim")
};

UENUM(BlueprintType)
enum EEquippedWeapon
{
	AR UMETA(DisplayName = "Assault Rifle"),
	Pistol UMETA(DisplayName = "Pistol"),
	SMG UMETA(DisplayName = "SMG"),
	SniperRifle UMETA(DisplayName = "Sniper Rifle"),
	MachineGun UMETA(DisplayName = "Machine Gun"),
	ShotGun UMETA(DisplayName = "ShotGun"),
	Knife UMETA(DisplayName = "Knife"),
	None UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum EAmmoType
{
	AssaultRifleType UMETA(DisplayName = "Assault Rifle"),
	PistolType UMETA(DisplayName = "Pistol"),
	SniperRifleType UMETA(DisplayName = "Sniper Rifle"),
	ShotgunType UMETA(DisplayName = "ShotGun"),
	NoneType UMETA(Display = "None")
};

UENUM(BlueprintType)
enum EFiringMode
{
	AUTO UMETA(DisplayName = "AUTO"),
	SEMI UMETA(DisplayName = "SEMI"),
	BURST UMETA(DisplayName = "AUTO")
};

UENUM(BlueprintType)
enum EGameModeNames
{
	//None UMETA(DisplayName = "None"),
	TeamDeathmatch UMETA(DisplayName = "Team Deathmatch"),
	CaptureTheFlag UMETA(DisplayName = "CaptureTheFlag"),
	Conquest UMETA(DisplayName = "Conquest")
};

/*UENUM(BlueprintType)
enum EGameMode
{
	//None UMETA(DisplayName = "None"),
	TeamDeathmatch UMETA(DisplayName = "Team Deathmatch"),
	CaptureTheFlag UMETA(DisplayName = "CaptureTheFlag"),
	Conquest UMETA(DisplayName = "Conquest")
};*/

UENUM(BlueprintType)
enum ELethalEquipment
{
	Claymore UMETA(DisplayName = "Claymore"),
	Grenade UMETA(DisplayName = "Grenade")
};

UENUM(BlueprintType)
enum EMatchState
{
	WaitingToStart UMETA(DisplayName = "WaitingToStart"),
	InProgress UMETA(DisplayName = "InProgress"),
	WaitingPostMatch UMETA(DisplayName = "WaitingPostMatch")
};

UENUM(BlueprintType)
enum EPerks
{
	HP UMETA(DisplayName = "HP"),
	Speed UMETA(DisplayName = "Speed"),
	Utility UMETA(DisplayName = "Utility"),
	Ammo UMETA(DisplayName = "Ammo")
};

UENUM(BlueprintType)
enum EPlayerClass
{
	Assault UMETA(DisplayName = "Assault"),
	Medic UMETA(DisplayName = "Medic"),
	Support UMETA(DisplayName = "Support")
};

UENUM(BlueprintType)
enum ERewardReasons
{
	Captured UMETA(DisplayName = "CAPTURED"),
	Secured UMETA(DisplayName = "SECURED"),
	Planted UMETA(DisplayName = "PLANTED"),
	Headshot UMETA(DisplayName = "HEADSHOT"),
	Suicide UMETA(DisplayName = "SUICIDE"),
	DoubleKill UMETA(DisplayName = "DOUBLE KILL"),
	TripleKill UMETA(DisplayName = "TRIPLE KILL"),
	FlagCaptured UMETA(DisplayName = "FLAG CAPTURED"),
	FlagTaken UMETA(DisplayName = "FLAG TAKEN"),
	FlagRecovered UMETA(DisplayName = "FLAG RECOVERED"),
};

UENUM(BlueprintType)
enum ESettingsLevel
{
	Low UMETA(DisplayName = "Low"),
	Medium UMETA(DisplayName = "Medium"),
	High UMETA(DisplayName = "High"),
	Ultra UMETA(DisplayName = "Ultra")
};

USTRUCT(BlueprintType)
struct FWeaponAmmo
{

	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponAmmo")
	TEnumAsByte<EAmmoType> AmmoType = EAmmoType::AssaultRifleType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponAmmo")
	int32 Count = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponAmmo")
	int32 MaxCount = 0;
};

USTRUCT(BlueprintType)
struct FLethalAmmo
{

	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LethalAmmo")
	TEnumAsByte<ELethalEquipment> LethalAmmoType = ELethalEquipment::Claymore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LethalAmmo")
	int32 Count = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LethalAmmo")
	int32 MaxCount = 0;
};
