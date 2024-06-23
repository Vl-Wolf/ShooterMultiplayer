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

	UPROPERTY()
	FVector Velocity = FVector::Zero();

	UPROPERTY(Replicated)
	float Damage = 0.0f;

	UPROPERTY()
	bool bClientSideOnly = false;

	UPROPERTY()
	bool bClientSideHitReg = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY()
	FVector StartLocation = FVector::Zero();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Details")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY()
	TArray<AActor*> ActorsHit;

	UPROPERTY()
	TSubclassOf<ASM_WeaponBase> WeaponRef;

	UPROPERTY()
	float ZVelocity = 60.0f;
	
	UFUNCTION()
	void BulletApplyDamage(APawn* AAInstigator, TSubclassOf<UDamageType> ADamageType, FHitResult& Hit);

	UFUNCTION()
	void HandleHit(FHitResult& Hit);

	UFUNCTION()
	void MoveBullet(FVector NewLocation, bool BlockingHit);

	UFUNCTION(Server, Reliable)
	void Server_ApplyPointDamage(APawn* AInstigator, TSubclassOf<UDamageType> ADamageType, FHitResult Hit);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
