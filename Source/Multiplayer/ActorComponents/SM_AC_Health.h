// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "SM_AC_Health.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, OldHealthValue, float, NewHealthValue);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MULTIPLAYER_API USM_AC_Health : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USM_AC_Health();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UPROPERTY()
	bool bHealthRegenEnabled = true;

	UPROPERTY()
	float HealthRegenInitialDelay = 4.0f;

	UPROPERTY()
	FTimerHandle StartRegenTimerHandle;

	UPROPERTY()
	FTimerHandle HealthRegenTimer;

	UPROPERTY()
	float RegenTickFrequency = 0.2f;

	UPROPERTY()
	float HealthPerTick = 1.0f;

	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float Health = 100.0f;

	UPROPERTY(ReplicatedUsing=OnRep_MaxHealth)
	float MaxHealth = 100.0f;

	UPROPERTY(ReplicatedUsing=OnRep_IsDead)
	bool bIsDead = false;

	UPROPERTY()
	float OldHealthValue = 0.0f;

	UPROPERTY()
	FOnPlayerDead OnPlayerDead;

	UPROPERTY()
	FOnHealthChanged OnHealthChanged;

public:
	
	UFUNCTION()
	virtual void OnRep_Health();

	UFUNCTION()
	virtual void OnRep_MaxHealth();

	UFUNCTION()
	virtual void OnRep_IsDead();

	UFUNCTION()
	float GetHealth();

	UFUNCTION()
	float GetMaxHealth();

	UFUNCTION()
	bool IsDead();

	UFUNCTION()
	float GetHealthNormalized();

	UFUNCTION()
	bool IsMaxHealth();

	UFUNCTION()
	void SetMaxHealth(float NewMaxHealth);

	UFUNCTION()
	float GetOldHealthValue();

	UFUNCTION()
	void SetOldHealthValue(float NewOldHealthValue);

	UFUNCTION()
	void HandleDamage(float Damage);

	UFUNCTION()
	void DamageActor(float NewHealth);

	UFUNCTION()
	void HealActor(float NewHealth);

	UFUNCTION()
	void ActivateHealthRegeneration();

	UFUNCTION()
	void StartHealthRegeneration();

	UFUNCTION()
	void HealTick();

	UFUNCTION()
	void Suicide();

	UFUNCTION(Server, Unreliable)
	void Server_KillPawn();

private:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
