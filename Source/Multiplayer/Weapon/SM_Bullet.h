// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SM_WeaponBase.h"
#include "GameFramework/Actor.h"
#include "SM_Bullet.generated.h"

UCLASS()
class MULTIPLAYER_API ASM_Bullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASM_Bullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	FVector Velocity = FVector::Zero();

	UPROPERTY()
	FVector StartLocation = FVector::Zero();

	UPROPERTY(Replicated)
	float Damage = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Details")
	UDamageType DamageType;

	UPROPERTY()
	TArray<AActor*> ActorsHit;

	UPROPERTY()
	TSubclassOf<ASM_WeaponBase*> WeaponRef;

	UPROPERTY()
	float ZVelocity = 60.0f;

	UPROPERTY()
	bool bClientSideOnly = false;

	UPROPERTY()
	bool bClientSideHitReg = false;

	UFUNCTION()
	void BulletApplyDamage(APawn* Instigator, TSubclassOf<UDamageType> DamageType, FHitResult* Hit);

	UFUNCTION()
	void HandleHit(FHitResult* Hit);

	UFUNCTION()
	void MoveBullet(FVector NewLocation, bool BlockingHit);

	UFUNCTION(Server, Reliable)
	void Server_ApplyPointDamage(APawn* Instigator, TSubclassOf<UDamageType> DamageType, FHitResult* Hit);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
