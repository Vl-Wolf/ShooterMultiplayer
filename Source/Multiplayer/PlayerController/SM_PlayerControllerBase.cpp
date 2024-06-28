// Fill out your copyright notice in the Description page of Project Settings.


#include "SM_PlayerControllerBase.h"

#include "EnhancedInputSubsystems.h"
#include "OnlineSubsystem.h"
#include "GameFramework/Character.h"
#include "Net/Subsystems/NetworkSubsystem.h"

void ASM_PlayerControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ACharacter* Character = Cast<ACharacter>(InPawn);

	if (Character)
	{
		Client_SetInitialControlRotation(InPawn->GetActorRotation());
	}
}

void ASM_PlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		UEnhancedInputLocalPlayerSubsystem EnhancedInputLocalPlayerSubsystem;
		EnhancedInputLocalPlayerSubsystem.AddMappingContext(ControllerMappingContext, 0);
	}
}

void ASM_PlayerControllerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASM_PlayerControllerBase::Client_SetInitialControlRotation_Implementation(FRotator NewRotation)
{
	SetControlRotation(NewRotation);
}