// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Multiplayer/EnumsAndStructs/SM_Types.h"
#include "SM_AC_ControllerSettings.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPerspectiveChanged);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MULTIPLAYER_API USM_AC_ControllerSettings : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USM_AC_ControllerSettings();

	UPROPERTY()
	FOnPerspectiveChanged OnPerspectiveChanged;

	UPROPERTY()
	bool bIsUseTPP = false;

	UFUNCTION()
	bool GetUseTPP();

	UFUNCTION()
	void SetUseTPP(bool bUseTPP);
	
	UFUNCTION()
	void GetMouseSettings(bool& bVerticalAxis_Inverted, bool& bHorizontalAxis_Inverted, float& Mouse_Sensitivity, float& Aim_Sensitivity);

	UFUNCTION()
	void AddFiringModeUsed(TEnumAsByte<EEquippedWeapon> Weapon, TEnumAsByte<EFiringMode> FiringMode);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool bIsVerticalAxis_Inverted = false;
	bool bIsHorizontalAxis_Inverted = false;
	float MouseSensitivity = 1.0f;
	float AimSensitivity = 1.0f;
	
	TMap<EEquippedWeapon, EFiringMode> FiringModesUsed;
};
