// Fill out your copyright notice in the Description page of Project Settings.


#include "SM_AC_ControllerSettings.h"


// Sets default values for this component's properties
USM_AC_ControllerSettings::USM_AC_ControllerSettings()
{
	PrimaryComponentTick.bCanEverTick = true;
}


bool USM_AC_ControllerSettings::GetUseTPP()
{
	return bIsUseTPP;
}

void USM_AC_ControllerSettings::SetUseTPP(bool bUseTPP)
{
	bIsUseTPP = bUseTPP;

	OnPerspectiveChanged.Broadcast();
}

void USM_AC_ControllerSettings::GetMouseSettings(bool& bVerticalAxis_Inverted, bool& bHorizontalAxis_Inverted,
	float& Mouse_Sensitivity, float& Aim_Sensitivity)
{
	bVerticalAxis_Inverted = bIsVerticalAxis_Inverted;
	bHorizontalAxis_Inverted = bIsHorizontalAxis_Inverted;
	Mouse_Sensitivity = MouseSensitivity;
	Aim_Sensitivity = AimSensitivity;
}

void USM_AC_ControllerSettings::AddFiringModeUsed(TEnumAsByte<EEquippedWeapon> Weapon,
	TEnumAsByte<EFiringMode> FiringMode)
{
	if (FiringModesUsed.Contains(Weapon))
	{
		FiringModesUsed.Remove(Weapon);
	}
	else
	{
		FiringModesUsed.Add(Weapon, FiringMode);
	}
}

// Called when the game starts
void USM_AC_ControllerSettings::BeginPlay()
{
	Super::BeginPlay();
	
}

