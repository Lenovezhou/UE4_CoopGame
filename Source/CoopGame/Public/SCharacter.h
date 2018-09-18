// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sweapon.h"
#include "SCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;
class USHealthComponent;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveRight(float value);

	void BeginCrouch();
	void EndCrouch();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
	UCameraComponent* cameracomp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* springarmcomp;


	UPROPERTY(Replicated)
	ASweapon* currentweapon;

	UPROPERTY(EditDefaultsOnly,Category = "Player")
	TSubclassOf<ASweapon> StarterWeaponClass;
	void Fire();
	void EndFire();


	bool wantzoom;

	float defaultFOV;
	float zoomedFOV;
	float currentFOV;

	UPROPERTY(EditDefaultsOnly,Category= "Player",meta = (ClampMin = 0.1,ClampMax = 100))
	float zoominterpspeed;

	void BeginZoom();
	void EndZoom();

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName;

	USHealthComponent* healthcomp;

	UFUNCTION()
		void OnhealthChanged(USHealthComponent* _healthcomponent, float health, float dealtahealth, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(BlueprintReadOnly,Category = "Health")
	bool bDie;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	
};
