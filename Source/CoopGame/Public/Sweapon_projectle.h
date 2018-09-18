// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sweapon.h"
#include "SBulletProjectle.h"
#include "Sweapon_projectle.generated.h"



class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class COOPGAME_API ASweapon_projectle : public ASweapon
{
	GENERATED_BODY()
	

protected:
	
	UPROPERTY(EditDefaultsOnly,Category= "Weapon")
	TSubclassOf<ASBulletProjectle> bulletactor;

public:
	virtual void Fire() override;
	
};
