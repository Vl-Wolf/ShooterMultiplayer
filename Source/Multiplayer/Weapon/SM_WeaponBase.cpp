// Fill out your copyright notice in the Description page of Project Settings.


#include "SM_WeaponBase.h"


// Sets default values
ASM_WeaponBase::ASM_WeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASM_WeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASM_WeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

