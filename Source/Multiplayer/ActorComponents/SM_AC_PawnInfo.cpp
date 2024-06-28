// Fill out your copyright notice in the Description page of Project Settings.


#include "SM_AC_PawnInfo.h"

#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USM_AC_PawnInfo::USM_AC_PawnInfo()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USM_AC_PawnInfo::OnRep_bIsUsingLethal()
{
	if (bIsUsingLethal)
	{
		OnStartUsingLethal.Broadcast();
	}
	else
	{
		OnStopUsingLethal.Broadcast();
	}
}

void USM_AC_PawnInfo::Server_SetAiming_Implementation(bool bAiming)
{
	bIsAiming = bAiming;
}

void USM_AC_PawnInfo::Server_SetShooting_Implementation(bool bShooting)
{
	bIsShooting = bShooting;
}

void USM_AC_PawnInfo::Server_SetRunning_Implementation(bool bRunning)
{
	bIsRunning = bRunning;
}

void USM_AC_PawnInfo::Server_SetUsingLethal_Implementation(bool bUsingLethal)
{
	bIsUsingLethal = bUsingLethal;
}

void USM_AC_PawnInfo::Client_DamageDone_Implementation(bool bIsDamagedActorACharacter)
{
	OnDamageDone.Broadcast(bIsDamagedActorACharacter);
}

void USM_AC_PawnInfo::SetAiming(bool bAiming)
{
	bIsAiming = bAiming;
	
	if (bIsAiming)
	{
		OnStartAiming.Broadcast();
	}
	else
	{
		OnStopAiming.Broadcast();
	}
}

bool USM_AC_PawnInfo::GetAiming()
{
	return bIsAiming;
}

void USM_AC_PawnInfo::SetShooting(bool bShooting)
{
	bIsShooting = bShooting;
	
	if (bIsShooting)
	{
		OnStartShooting.Broadcast();
	}
	else
	{
		OnStopShooting.Broadcast();
	}
}

bool USM_AC_PawnInfo::GetShooting()
{
	return bIsShooting;
}

void USM_AC_PawnInfo::SetRunning(bool bRunning)
{
	bIsRunning = bRunning;
	
	if (bIsRunning)
	{
		OnStartRunning.Broadcast();
	}
	else
	{
		OnStopRunning.Broadcast();
	}
}

bool USM_AC_PawnInfo::GetRunning()
{
	return bIsRunning;
}

void USM_AC_PawnInfo::SetUsingLetha(bool bUsingLethal)
{
	bIsUsingLethal = bUsingLethal;
	
	if (bIsUsingLethal)
	{
		OnStartUsingLethal.Broadcast();
	}
	else
	{
		OnStartUsingLethal.Broadcast();
	}
}

bool USM_AC_PawnInfo::GetUsingLethal()
{
	return bIsUsingLethal;
}

void USM_AC_PawnInfo::SetCrouching(bool bCrouching)
{
	bIsCrouching = bCrouching;
	
	if (bIsCrouching)
	{
		OnStartCrouching.Broadcast();
	}
	else
	{
		OnStopCrouching.Broadcast();
	}
}

void USM_AC_PawnInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(USM_AC_PawnInfo, bIsRunning, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(USM_AC_PawnInfo, bIsAiming, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(USM_AC_PawnInfo, bIsShooting, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(USM_AC_PawnInfo, bIsUsingLethal, COND_SkipOwner);
}


