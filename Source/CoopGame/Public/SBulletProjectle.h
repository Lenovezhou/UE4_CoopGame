// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "SBulletProjectle.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class COOPGAME_API ASBulletProjectle : public AActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, category = "ExposionFx")
		UParticleSystem* ExplosionEmitter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "ExposionFx")
		TSubclassOf<UDamageType> damageclass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, category = "ExposionFx")
		TArray<AActor*> ignoractors;

	UPROPERTY(EditDefaultsOnly, category = "ExposionFx")
		USoundBase* sound;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FTimerHandle timerhandle;




	UFUNCTION()
		void DestroyMe();



public:

	ASBulletProjectle();

	/** called when projectile hits something */
	//UFUNCTION()
	//	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

};
