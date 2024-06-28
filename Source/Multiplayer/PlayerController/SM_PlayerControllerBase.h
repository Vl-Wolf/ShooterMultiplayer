// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "SM_PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API ASM_PlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

protected:

	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	UFUNCTION(Client, Reliable)
	void Client_SetInitialControlRotation(FRotator NewRotation);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Input", meta = (AllowPrivateAccess = true))
	UInputMappingContext* ControllerMappingContext = nullptr;

	
};

