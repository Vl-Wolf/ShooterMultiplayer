// Fill out your copyright notice in the Description page of Project Settings.


#include "SM_Bullet.h"

#include "Kismet/GameplayStatics.h"


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

void ASM_Bullet::BulletApplyDamage(APawn* Instigator, TSubclassOf<UDamageType> DamageType, FHitResult* Hit)
{

	//Change Base damage from weapon through inventory component
	
	UGameplayStatics::ApplyPointDamage(Hit->GetActor(), 50.0f, Hit->TraceEnd - Hit->TraceStart,
		*Hit, Instigator->Controller, Instigator, DamageType);
}

void ASM_Bullet::HandleHit(FHitResult* Hit)
{
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

void ASM_Bullet::Server_ApplyPointDamage_Implementation(APawn* Instigator, TSubclassOf<UDamageType> DamageType, FHitResult* Hit)
{
	BulletApplyDamage(Instigator, DamageType, Hit);
}

// Called every frame
void ASM_Bullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 

	
}

