// Fill out your copyright notice in the Description page of Project Settings.


#include "SM_Bullet.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASM_Bullet::ASM_Bullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASM_Bullet::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASM_Bullet::BulletApplyDamage(APawn* AAInstigator, TSubclassOf<UDamageType> ADamageType, FHitResult& Hit)
{

	//Change Base damage from weapon through inventory component
	
	UGameplayStatics::ApplyPointDamage(Hit.GetActor(), 50.0f, Hit.TraceEnd - Hit.TraceStart,
		Hit, AAInstigator->Controller, AAInstigator, ADamageType);
}

void ASM_Bullet::HandleHit(FHitResult& Hit)
{
	if (WeaponRef)
	{
		ASM_WeaponBase* WeaponBase = Cast<ASM_WeaponBase>(WeaponRef);
		if (bClientSideOnly)
		{
			WeaponBase->PlayEffects(Hit);
		}
		else
		{
			WeaponBase->Server_PlayImpactEffects(Hit);
		}
	}
}

void ASM_Bullet::MoveBullet(FVector NewLocation, bool BlockingHit)
{
	SetActorLocation(NewLocation);
	StartLocation = NewLocation;
	
	Velocity.X = Velocity.X;
	Velocity.Y = Velocity.Y;
	Velocity.Z = Velocity.Z - ZVelocity;

	if (BlockingHit)
	{
		Destroy();
	}
}

void ASM_Bullet::Server_ApplyPointDamage_Implementation(APawn* AInstigator, TSubclassOf<UDamageType> ADamageType, FHitResult Hit)
{
	BulletApplyDamage(AInstigator, ADamageType, Hit);
}

void ASM_Bullet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASM_Bullet, Damage);
}

// Called every frame
void ASM_Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 

	
}

