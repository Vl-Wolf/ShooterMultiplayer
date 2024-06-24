// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SM_BaseLethalEquipment.generated.h"

UCLASS()
class MULTIPLAYER_API ASM_BaseLethalEquipment : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASM_BaseLethalEquipment();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
