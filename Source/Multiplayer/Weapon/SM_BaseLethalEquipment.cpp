// Fill out your copyright notice in the Description page of Project Settings.


#include "SM_BaseLethalEquipment.h"


// Sets default values
ASM_BaseLethalEquipment::ASM_BaseLethalEquipment()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASM_BaseLethalEquipment::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASM_BaseLethalEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

