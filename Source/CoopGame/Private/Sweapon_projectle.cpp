// Fill out your copyright notice in the Description page of Project Settings.

#include "Sweapon_projectle.h"
#include "Kismet/GameplayStatics.h"
#include "Public/SBulletProjectle.h"
#include "Engine/World.h"


void ASweapon_projectle::Fire()
{
	AActor* myowner = GetOwner();
	if (myowner && nullptr != bulletactor)
	{
		FVector eyelocation;
		FRotator eyerotation;

		myowner->GetActorEyesViewPoint(eyelocation, eyerotation);

		FVector spawnlocaltion = meshcomponent->GetSocketLocation(MuzzelSocketName);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<ASBulletProjectle>(bulletactor, spawnlocaltion, eyerotation, ActorSpawnParams);

	}

}