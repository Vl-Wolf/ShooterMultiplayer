// Fill out your copyright notice in the Description page of Project Settings.


#include "SM_AC_Health.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


USM_AC_Health::USM_AC_Health()
{
	PrimaryComponentTick.bCanEverTick = true;

	//bReplicates = true;
}

void USM_AC_Health::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Health = MaxHealth;
	}
	else
	{
		OldHealthValue = MaxHealth;
	}
		
}

void USM_AC_Health::OnRep_Health()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		if (PlayerController->IsLocalController())
		{
			OnHealthChanged.Broadcast(OldHealthValue, Health);
			OldHealthValue = Health;
		}
	}
}

void USM_AC_Health::OnRep_MaxHealth()
{
}

void USM_AC_Health::OnRep_IsDead()
{
	OnPlayerDead.Broadcast();
}

float USM_AC_Health::GetHealth()
{
	return Health;
}

float USM_AC_Health::GetMaxHealth()
{
	return MaxHealth;
}

bool USM_AC_Health::IsDead()
{
	return bIsDead;
}

float USM_AC_Health::GetHealthNormalized()
{
	return UKismetMathLibrary::NormalizeToRange(Health, 0.0f, MaxHealth);
}

bool USM_AC_Health::IsMaxHealth()
{
	if (Health == MaxHealth)
	{
		return true;
	}

	return false;
}

void USM_AC_Health::SetMaxHealth(float NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
}

float USM_AC_Health::GetOldHealthValue()
{
	return OldHealthValue;
}

void USM_AC_Health::SetOldHealthValue(float NewOldHealthValue)
{
	this->OldHealthValue = NewOldHealthValue;
}

void USM_AC_Health::HandleDamage(float Damage)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (Damage > 0.0f)
		{
			DamageActor(Damage);
		}
		else
		{
			HealActor(Damage);
		}

		GetOwner()->ForceNetUpdate();
	}
}

void USM_AC_Health::DamageActor(float NewHealth)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		this->Health = UKismetMathLibrary::FClamp(this->Health - NewHealth, 0.0f, MaxHealth);

		if (this->Health == 0.0f)
		{
			bIsDead = true;
			if (GetWorld())
			{
				if (StartRegenTimerHandle.IsValid())
					GetWorld()->GetTimerManager().ClearTimer(StartRegenTimerHandle);

				if (HealthRegenTimer.IsValid())
					GetWorld()->GetTimerManager().ClearTimer(HealthRegenTimer);
			}
		}
		else
		{
			if (bHealthRegenEnabled)
				if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
					ActivateHealthRegeneration();
		}
		
		GetOwner()->ForceNetUpdate();
	}
}

void USM_AC_Health::HealActor(float NewHealth)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		this->Health = UKismetMathLibrary::FClamp(this->Health - NewHealth, 0.0f, MaxHealth);

		if (IsMaxHealth())
		{
			if (GetWorld())
			{
				if (StartRegenTimerHandle.IsValid())
					GetWorld()->GetTimerManager().ClearTimer(StartRegenTimerHandle);

				if (HealthRegenTimer.IsValid())
					GetWorld()->GetTimerManager().ClearTimer(HealthRegenTimer);
			}
		}
		
		GetOwner()->ForceNetUpdate();
	}
}

void USM_AC_Health::ActivateHealthRegeneration()
{
	if (GetWorld())
	{
		if (StartRegenTimerHandle.IsValid())
			GetWorld()->GetTimerManager().ClearTimer(StartRegenTimerHandle);

		if (HealthRegenTimer.IsValid())
			GetWorld()->GetTimerManager().ClearTimer(HealthRegenTimer);

		GetWorld()->GetTimerManager().SetTimer(StartRegenTimerHandle, this, &USM_AC_Health::StartHealthRegeneration,
											HealthRegenInitialDelay, false);	
	}
}

void USM_AC_Health::StartHealthRegeneration()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(HealthRegenTimer, this, &USM_AC_Health::HealTick,
											RegenTickFrequency, true);
	}
}

void USM_AC_Health::HealTick()
{
	HealActor(HealthPerTick * -1.0f);
}

void USM_AC_Health::Suicide()
{
	Server_KillPawn();
}

void USM_AC_Health::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USM_AC_Health, Health);
	DOREPLIFETIME(USM_AC_Health, MaxHealth);
	DOREPLIFETIME(USM_AC_Health, bIsDead);
}

void USM_AC_Health::Server_KillPawn_Implementation()
{
	UGameplayStatics::ApplyDamage(GetOwner(), 10000.0f, nullptr, GetOwner(), nullptr);
}


