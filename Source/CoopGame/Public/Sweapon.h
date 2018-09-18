// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sweapon.generated.h"


class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()
public:
	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> surfacetype;

	UPROPERTY()
		FVector_NetQuantize TraceTo;
};


UCLASS()
class COOPGAME_API ASweapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASweapon();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
	USkeletalMeshComponent* meshcomponent;
protected:
	virtual void BeginPlay() override;

	float basedamage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Weapon")
	TSubclassOf<UDamageType> damagetypeclass;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Weapon")
	UParticleSystem* MuzzelEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshVulnerableEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FName MuzzelSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FName TracerEffectTargetName;

	UPROPERTY(EditDefaultsOnly,Category = "Weapon")
	TSubclassOf<UCameraShake> camerashakeanim;

	void PlayFireEffect(FVector TrancerEndPos);

	void PlayImpactEffect(EPhysicalSurface surfacetype, FVector ImpactPoint);

	virtual void Fire();


	FTimerHandle fireratehandle;

	float firerate;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float firepreminitNUM;

	float lastfiretime;

	UFUNCTION(Server,Reliable,WithValidation)
	void ServerFire();


	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
		void OnRep_HitScanTrace();

public:	
	void StartFire();
	void EndFire();
	
};
