// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Multiplayer/ActorComponents/SM_AC_Health.h"
#include "Multiplayer/ActorComponents/SM_AC_InventorySystem.h"
#include "Multiplayer/ActorComponents/SM_AC_PawnInfo.h"
#include "MultiplayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartFiring);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopFiring);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPerspectiveChange);

UCLASS(config=Game)
class AMultiplayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess))
	USkeletalMeshComponent* MeshFP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess))
	USM_AC_Health* AC_Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess))
	USM_AC_InventorySystem* Ac_InventorySystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess))
	USM_AC_PawnInfo* Ac_PawnInfo;

public:
	
	AMultiplayerCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

