// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SM_AC_PawnInfo.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopAimimg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartShooting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopShooting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartRunning);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopRunning);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartUsingLethal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopUsingLethal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartCrouching);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopCrouching);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageDone, bool, bIsDamageDone);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MULTIPLAYER_API USM_AC_PawnInfo : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USM_AC_PawnInfo();

	UPROPERTY()
	FOnStartAiming OnStartAiming;

	UPROPERTY()
	FOnStopAimimg OnStopAiming;

	UPROPERTY()
	FOnStartShooting OnStartShooting;

	UPROPERTY()
	FOnStopShooting OnStopShooting;

	UPROPERTY()
	FOnStartRunning OnStartRunning;

	UPROPERTY()
	FOnStopRunning OnStopRunning;

	UPROPERTY()
	FOnStartUsingLethal OnStartUsingLethal;

	UPROPERTY()
	FOnStopUsingLethal OnStopUsingLethal;

	UPROPERTY()
	FOnStartCrouching OnStartCrouching;

	UPROPERTY()
	FOnStopCrouching OnStopCrouching;

	UPROPERTY()
	FOnDamageDone OnDamageDone;

	UPROPERTY(Replicated)
	bool bIsRunning = false;

	UPROPERTY(Replicated)
	bool bIsAiming = false;

	UPROPERTY(Replicated)
	bool bIsShooting = false;

	UPROPERTY(ReplicatedUsing=OnRep_bIsUsingLethal)
	bool bIsUsingLethal = false;

	UPROPERTY()
	bool bIsCrouching = false;

	UFUNCTION()
	virtual void OnRep_bIsUsingLethal();

	UFUNCTION(Server, Reliable)
	void Server_SetAiming(bool bAiming);

	UFUNCTION(Server, Reliable)
	void Server_SetShooting(bool bShooting);

	UFUNCTION(Server, Reliable)
	void Server_SetRunning(bool bRunning);

	UFUNCTION(Server, Reliable)
	void Server_SetUsingLethal(bool bUsingLethal);

	UFUNCTION(Client, Reliable)
	void Client_DamageDone(bool bIsDamagedActorACharacter);

	UFUNCTION()
	void SetAiming(bool bAiming);

	UFUNCTION()
	bool GetAiming();

	UFUNCTION()
	void SetShooting(bool bShooting);

	UFUNCTION()
	bool GetShooting();

	UFUNCTION()
	void SetRunning(bool bRunning);

	UFUNCTION()
	bool GetRunning();

	UFUNCTION()
	void SetUsingLetha(bool bUsingLethal);

	UFUNCTION()
	bool GetUsingLethal();

	UFUNCTION()
	void SetCrouching(bool bCrouching);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
