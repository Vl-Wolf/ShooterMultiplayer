// Fill out your copyright notice in the Description page of Project Settings.


#include "SM_WeaponBase.h"

#include "SM_Bullet.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Multiplayer/ActorComponents/SM_AC_Health.h"
#include "Perception/AISense_Hearing.h"


// Sets default values
ASM_WeaponBase::ASM_WeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	MeshFPP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshFPP"));
	MeshFPP->SetupAttachment(RootComponent);
	MeshFPP->SetCollisionProfileName(TEXT("NoCollision"));
	MeshFPP->SetGenerateOverlapEvents(false);

	MeshTPP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshTPP"));
	MeshTPP->SetupAttachment(RootComponent);
	MeshTPP->SetCollisionProfileName(TEXT("NoCollision"));
	MeshTPP->SetGenerateOverlapEvents(false);
}

// Called when the game starts or when spawned
void ASM_WeaponBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASM_WeaponBase::ReduceAmmo()
{
	ActualAmmo--;
	ActualAmmo = FMath::Clamp(ActualAmmo, 0, AmmoPerMag);
	OnAmmoChanged.Broadcast();
}

void ASM_WeaponBase::SpawnBullet(FVector SpawnLocation, FVector Velocity, APawn* AInstigator,
	FRotator SpawnRotation, float Spread, bool bIsClientSideOnly)
{
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = AInstigator;
	ActorSpawnParameters.Instigator = AInstigator;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined;
	ActorSpawnParameters.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;
	
	
	if (bIsShotgun)
	{
		for (int i = 1; i < ShotgunProjectilesPerShot; i++)
		{
			ASM_Bullet* Bullet = Cast<ASM_Bullet>(GetWorld()->SpawnActor<ASM_Bullet>(SpawnLocation, SpawnRotation, ActorSpawnParameters));
			
			Bullet->Velocity = FMath::VRandCone(Velocity, Spread) * Velocity;
			Bullet->Damage = this->Damage;
			Bullet->bClientSideOnly = bIsClientSideOnly;
			Bullet->bClientSideHitReg = bUseClientHitReg;
		}
	}
	else
	{
		ASM_Bullet* Bullet = Cast<ASM_Bullet>(GetWorld()->SpawnActor<ASM_Bullet>(SpawnLocation, SpawnRotation, ActorSpawnParameters));
		
		Bullet->Velocity = FMath::VRandCone(Velocity, Spread) * Velocity;
		Bullet->Damage = this->Damage;
		Bullet->bClientSideOnly = bIsClientSideOnly;
		Bullet->bClientSideHitReg = bUseClientHitReg;
	}
}

bool ASM_WeaponBase::CheckBulletPath()
{
	FVector TraceStartRef = MeshTPP->GetSocketLocation(TEXT("Muzzle"));
	FVector ForwardVectorRef = UKismetMathLibrary::GetForwardVector(GetOwner()->GetInstigatorController()->GetControlRotation());
	FVector WorldLocation = GetOwner()->GetComponentByClass(UCameraComponent::StaticClass())->GetOwner()->GetActorLocation(); //Debug
	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStartRef, WorldLocation + (ForwardVectorRef * 3000.0f),
									TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None,
											HitResult, true);

	APawn* Pawn = Cast<APawn>(HitResult.GetActor());
	if (!Pawn)
	{
		FVector Length = HitResult.ImpactPoint - MeshTPP->GetComponentLocation();
		if (HitResult.bBlockingHit && UKismetMathLibrary::VSize(Length) < 120.0f)
		{
			APawn* PawnOwner = Cast<APawn>(GetOwner());
			
			if (HasAuthority())
			{
				Server_FireInstantBullet(TraceStartRef, ForwardVectorRef, PawnOwner, 0.03f);
			}
			else
			{
				FireInstant(TraceStartRef, ForwardVectorRef, PawnOwner, 0.0f, true);
			}

			return true;
			
		}
	}

	return false;
	
}

void ASM_WeaponBase::FireInstant(FVector TraceStart, FVector ForwardVector, APawn* AAInstigator, float Spread,
	bool bIsClientSideOnly)
{
	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsHitted;
	
	if (bIsShotgun)
	{
		for (int i = 1; i < ShotgunProjectilesPerShot; i++)
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(AAInstigator);
			FVector TraceEnd = FMath::VRandCone(ForwardVector, Spread) * WeaponRange + TraceStart;
			
			UKismetSystemLibrary::LineTraceMulti(GetWorld(), TraceStart, TraceEnd, TraceTypeQuery1,
				true, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true);

			for (int32 j = 0; j < HitResults.Num() - 1; j++)
			{
				HitResults.Add(HitResults[j]);
				if (HitResults[j].GetActor())
				{
					if (!bIsClientSideOnly)
					{
						if (!ActorsHitted.Contains(HitResults[j].GetActor()))
						{
							ActorsHitted.AddUnique(HitResults[j].GetActor());

							if (HitResults[j].GetActor()->GetComponentByClass(USM_AC_Health::StaticClass()))
							{
								
								Server_InstantBulletHit(HitResults[j], static_cast<float>(1.0f / (j + 1)), AAInstigator); //Debug
							}
						}
					}
				}
			}
		}

		if (bIsClientSideOnly)
		{
			for (FHitResult CurrentHit : HitResults)
			{
				PlayEffects(CurrentHit);
			}
		}
		else
		{
			Server_PlayHitImpactEffect(HitResults);

			if (bUseClientHitReg)
			{
				for (FHitResult CurrentHit : HitResults)
				{
					PlayEffects(CurrentHit);
				}
			}
		}
	}
}

void ASM_WeaponBase::GetLineTraceInfo(FVector& TraceStart, FVector& ForwardVector)
{
	UCameraComponent* CameraComponent = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
	FVector CameraLocation = CameraComponent->GetComponentLocation();
	
	ForwardVector = UKismetMathLibrary::GetForwardVector(GetOwner()->GetInstigatorController()->GetControlRotation());
	TraceStart = CameraLocation + (ForwardVector * 200.0f);
}

void ASM_WeaponBase::GetSpawnBulletInfo(FVector& SpawnLocation, FVector& AVelocity, FRotator& SpawnRotation)
{
	UCameraComponent* CameraComponent = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
	FVector CameraLocation = CameraComponent->GetComponentLocation();
	
	AVelocity = UKismetMathLibrary::GetForwardVector(GetOwner()->GetInstigatorController()->GetControlRotation());
	SpawnLocation = CameraLocation + (AVelocity * 200.0f);
	SpawnRotation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetControlRotation();
}

void ASM_WeaponBase::OnInstantBulletHit(FHitResult& Hit, float DmgMultiplier, APawn* AAAInstigator)
{
	// Refactor when will AC Inventory

	float BaseDamage = DmgMultiplier * Damage;;

	if (Hit.BoneName == FName("Head"))
	{
		BaseDamage *= HeadshotMultiplier;
	}
	
	UGameplayStatics::ApplyPointDamage(Hit.GetActor(), BaseDamage, Hit.TraceStart - Hit.TraceEnd, Hit, AAAInstigator->GetController(), AAAInstigator, this->DamageType);
}

void ASM_WeaponBase::PlayEffects_Implementation(FHitResult Hit)
{
	// Blueprint
}

void ASM_WeaponBase::CreateCompassMarker()
{
	APawn* Pawn = Cast<APawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Pawn)
	{
		if (!bHasSuppressor)
		{
			ACharacter* Character = Cast<ACharacter>(GetOwner());
			if (Character->GetPlayerState())
			{
				if (Character)
				{
					if (Pawn->GetPlayerState())
					{
						FVector Length = Pawn->GetActorLocation() - GetOwner()->GetActorLocation();
						// Refactor when will AC Team
						/*if (UKismetMathLibrary::VSize(Length) < 5000.0f)
						{
							
						}*/
					}		
				}
			}
		}
	}
}

void ASM_WeaponBase::OnRep_ActualAmmo()
{
	OnAmmoChanged.Broadcast();
}

void ASM_WeaponBase::ReloadWeapon(int32 AddedAmmo)
{
	ActualAmmo = UKismetMathLibrary::Clamp(ActualAmmo + AddedAmmo, 0, AmmoPerMag);
	OnAmmoChanged.Broadcast();
	Server_SetActualAmmo(ActualAmmo);
}

void ASM_WeaponBase::ChangeFiringMode()
{
	int32 ArrayLength = FiringModes.Num() - 1;
	int32 FiringIndex = FiringModes.Find(ActualFiringMode);
	if (ArrayLength == FiringIndex)
	{
		ActualFiringMode = FiringModes[0];
	}
	else
	{
		ActualFiringMode = FiringModes[FiringIndex + 1]; // Debug
	}
}

void ASM_WeaponBase::UpdateWeaponVisibility(bool bIsThirdPerson)
{
	MeshFPP->SetOwnerNoSee(bIsThirdPerson);
	MeshTPP->SetOwnerNoSee(!bIsThirdPerson);
	MeshTPP->SetVisibility(!bIsThirdPerson, false);
}

void ASM_WeaponBase::Server_SetActualAmmo_Implementation(int32 NewActualAmmo)
{
	this->ActualAmmo = NewActualAmmo;
}

void ASM_WeaponBase::Multicast_PlayShotEffects_Implementation()
{
	//Debug
	
	if (!IsRunningDedicatedServer())
	{
		if (!(GetOwner() == UGameplayStatics::GetPlayerPawn(GetOwner(), 0)))
		{
			if (MeshTPP)
			{
				FVector SpawnLocation = FVector::Zero();
				FTransform MuzzleTransform = MeshTPP->GetSocketTransform(FName("Muzzle"));
				FVector EmitterScale;
				EmitterScale.X = FMath::RandRange(0.4f, 0.5f);
				EmitterScale.Y = FMath::RandRange(0.4f, 0.5f);
				EmitterScale.Z = FMath::RandRange(0.4f, 0.5f);
				
				UGameplayStatics::SpawnEmitterAttached(MuzzleFlashEmitter, MeshTPP, FName("Muzzle"), SpawnLocation,
					MuzzleTransform.Rotator(), EmitterScale);

				PlayWeaponFireSound();
				CreateCompassMarker();
			}
		}
	}
}

void ASM_WeaponBase::LocalPlayEffects()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character->GetPlayerState())
	{
		//!Character->GetPlayerState()->bIsABot;
		if (!Character->GetPlayerState()->IsABot())
		{
			PlayWeaponShotAnimation();
			PlayWeaponFireSound();

			//Refactor check TPP or FPP

			if (bIsTPP)
			{
				if (MeshTPP)
				{
					if (!(WeaponType == SniperRifle))
					{
						if (MeshTPP->IsVisible())
						{
							UGameplayStatics::SpawnEmitterAttached(MuzzleFlashEmitter, MeshTPP, FName("Muzzle"),
								FVector::Zero(), FRotator::ZeroRotator, FVector(0.8f, 0.8f, 0.8f));
						}
					}
				}
			}
			else
			{
				if (MeshFPP)
				{
					if (MeshFPP->IsVisible())
					{
						FTransform MuzzleTransform = MeshTPP->GetSocketTransform(FName("Muzzle"));
						FVector EmitterScale;
						EmitterScale.X = FMath::RandRange(0.4f, 0.5f);
						EmitterScale.Y = FMath::RandRange(0.4f, 0.5f);
						EmitterScale.Z = FMath::RandRange(0.4f, 0.5f);
				
						UGameplayStatics::SpawnEmitterAttached(MuzzleFlashEmitter, MeshTPP, FName("Muzzle"), FVector::Zero(),
							MuzzleTransform.Rotator(), EmitterScale);
					}
				}
			}
		}
	}
}

void ASM_WeaponBase::Multicast_PlayImpactEffects_Implementation(const FHitResult& HitResult)
{
	if (!IsRunningDedicatedServer())
	{
		if (HasAuthority())
		{
			PlayEffects(HitResult);
		}
		else
		{
			if (!(GetOwner() == UGameplayStatics::GetPlayerPawn(GetOwner(), 0)))
			{
				PlayEffects(HitResult);
			}
		}
	}
}

void ASM_WeaponBase::Server_PlayImpactEffects_Implementation(const FHitResult& HitResult)
{
	Multicast_PlayImpactEffects(HitResult);
}

void ASM_WeaponBase::Server_PlayShootEffects_Implementation()
{
	Multicast_PlayShotEffects();
}

void ASM_WeaponBase::Multicast_PlayMultipleImpactEffects_Implementation(const TArray<FHitResult>& HitResults)
{
	if (!IsRunningDedicatedServer())
	{
		if (HasAuthority())
		{
			for (FHitResult CurrentHit : HitResults)
			{
				PlayEffects(CurrentHit);
			}
		}
		else
		{
			if (!(GetOwner() == UGameplayStatics::GetPlayerPawn(GetOwner(), 0)))
			{
				for (FHitResult CurrentHit : HitResults)
				{
					PlayEffects(CurrentHit);
				}
			}
		}
	}
}

void ASM_WeaponBase::Server_PlayHitImpactEffect_Implementation(const TArray<FHitResult>& HitResults)
{
	Multicast_PlayMultipleImpactEffects(HitResults);
}

void ASM_WeaponBase::PlayShootShake()
{
	if (UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		if (UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
		{
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(FireShake, 1.0f);
		}
	}
}

void ASM_WeaponBase::PlayWeaponFireSound()
{
	if (MeshTPP)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, MeshTPP->GetComponentLocation(),
			FRotator::ZeroRotator, 1.0f, 1.0f, 0.0f, nullptr,
			nullptr, UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	}
}

void ASM_WeaponBase::PlayWeaponShotAnimation()
{
	if (MeshFPP)
	{
		MeshFPP->PlayAnimation(ShootAnimation, false);
	}
}

void ASM_WeaponBase::Shoot(float Spread)
{
	if (bUseClientHitReg)
	{
		ClientSideHitReg(Spread);
	}
	else
	{
		ServerSideHitReg(Spread);
	}
}

void ASM_WeaponBase::Server_FireBullet_Implementation(FVector SpawnLocation, FVector Velocity, APawn* AInstigator,
	FRotator SpawnRotation, float Spread)
{
	SpawnBullet(SpawnLocation, Velocity, AInstigator, SpawnRotation, Spread, false);

	//UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), NoiseLoudness, GetOwner(), NoiseRange);

	ReduceAmmo();
}

void ASM_WeaponBase::Server_FireInstantBullet_Implementation(FVector TraceStart, FVector ForwardVector,
	APawn* AInstigator, float Spread)
{
	FireInstant(TraceStart, ForwardVector, AInstigator, Spread, false);

	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), NoiseLoudness, GetOwner(), NoiseRange);

	ReduceAmmo();
}

void ASM_WeaponBase::Server_InstantBulletHit_Implementation(FHitResult Hit, float DmgMultiplier, APawn* AInstigator)
{
	OnInstantBulletHit(Hit, DmgMultiplier, AInstigator);
}

void ASM_WeaponBase::ClientSideHitReg(float Spread)
{
	if (CheckBulletPath())
	{
		LocalPlayEffects();
		Server_PlayShootEffects();
		Server_ReduceAmmo();

		ACharacter* Character = Cast<ACharacter>(GetOwner());

		
		if (Character)
			if (Character->GetPlayerState())
				//!Character->GetPlayerState()->bIsABot
				if (!Character->GetPlayerState()->IsABot())
					PlayShootShake();
	}
	else
	{
		APawn* Pawn = Cast<APawn>(GetOwner());
		if (bHitScanBullets)
		{
			FVector TraceStart;
			FVector ForwardVector;
			GetLineTraceInfo(TraceStart, ForwardVector);
			FireInstant(TraceStart, ForwardVector, Pawn, Spread, false);
		}
		else
		{
			FVector SpawnLocation;
			FVector Velocity;
			FRotator SpawnRotation;
			GetSpawnBulletInfo(SpawnLocation, Velocity, SpawnRotation);
			SpawnBullet(SpawnLocation, Velocity, Pawn, SpawnRotation, Spread, false);
		}

		LocalPlayEffects();
		Server_PlayShootEffects();
		Server_ReduceAmmo();

		ACharacter* Character = Cast<ACharacter>(GetOwner());
		
		if (Character)
			if (Character->GetPlayerState())
				//!Character->GetPlayerState()->bIsABot
				if (!Character->GetPlayerState()->IsABot())
					PlayShootShake();
	}
}

void ASM_WeaponBase::ServerSideHitReg(float Spread)
{
	if (CheckBulletPath())
	{
		LocalPlayEffects();
		Server_PlayShootEffects();
		
	}
	else
	{
		if (bHitScanBullets)
		{
			FVector TraceStart;
			FVector ForwardVector;
			GetLineTraceInfo(TraceStart, ForwardVector);
			Server_FireInstantBullet(TraceStart, ForwardVector, UGameplayStatics::GetPlayerPawn(GetWorld(), 0), Spread);
			if (HasAuthority())
			{
				/*LocalPlayEffects();
				Server_PlayShootEffects();*/
			}
			else
			{
				FireInstant(TraceStart, ForwardVector, UGameplayStatics::GetPlayerPawn(GetWorld(), 0), Spread, true);
				/*LocalPlayEffects();
				Server_PlayShootEffects();*/
			}
		}
		else
		{
			FVector SpawnLocation;
			FVector Velocity;
			FRotator SpawnRotation;
			GetSpawnBulletInfo(SpawnLocation, Velocity, SpawnRotation);
			Server_FireBullet(SpawnLocation, Velocity, UGameplayStatics::GetPlayerPawn(GetWorld(), 0), SpawnRotation, Spread);
			if (HasAuthority())
			{
				/*LocalPlayEffects();
				Server_PlayShootEffects();*/
			}
			else
			{
				SpawnBullet(SpawnLocation, Velocity, UGameplayStatics::GetPlayerPawn(GetWorld(), 0), SpawnRotation, Spread, true);
				/*LocalPlayEffects();
				Server_PlayShootEffects();*/
			}
		}

		LocalPlayEffects();
		Server_PlayShootEffects();
	}
	
}

void ASM_WeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASM_WeaponBase, ActualAmmo);
	DOREPLIFETIME(ASM_WeaponBase, PickupWeaponAmmo);
	DOREPLIFETIME(ASM_WeaponBase, InitialValues);
	DOREPLIFETIME(ASM_WeaponBase, DamageType);
	//DOREPLIFETIME(ASM_WeaponBase, )
}

void ASM_WeaponBase::Server_BulletApplyDamage_Implementation(const int32 ActorsHitted, APawn* InstigatorCharacter,
                                                             const FHitResult& HitInfo)
{

	float BaseDamage = Damage * (1.0f / ActorsHitted);

	if (HitInfo.BoneName == FName("Head"))
	{
		BaseDamage *= HeadshotMultiplier; 
	}

	
	UGameplayStatics::ApplyPointDamage(HitInfo.GetActor(), BaseDamage, HitInfo.TraceStart - HitInfo.TraceEnd,
		HitInfo, InstigatorCharacter->GetController(), InstigatorCharacter, this->DamageType);

}

void ASM_WeaponBase::Server_ReduceAmmo_Implementation()
{
	ReduceAmmo();
}
