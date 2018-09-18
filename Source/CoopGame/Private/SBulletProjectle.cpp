// Fill out your copyright notice in the Description page of Project Settings.

#include "SBulletProjectle.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"


// Sets default values
ASBulletProjectle::ASBulletProjectle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("spherecollisioncomp"));
	CollisionComp->SetSimulatePhysics(true);
	CollisionComp->InitSphereRadius(5.0f);

	CollisionComp->OnComponentHit.AddDynamic(this, &ASBulletProjectle::OnHit);

	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("projectilemovementcomp"));
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	InitialLifeSpan = 1.f;
}


void ASBulletProjectle::BeginPlay()
{
	GetWorldTimerManager().ClearTimer(timerhandle);
	GetWorldTimerManager().SetTimer(timerhandle, this,&ASBulletProjectle::DestroyMe, 1);
}

void ASBulletProjectle::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	DestroyMe();
}

//void ASBulletProjectle::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//	// Only add impulse and destroy projectile if we hit a physics
//	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
//	{
//		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
//
//		if (ExplosionEmitter)
//		{
//			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEmitter, GetActorLocation());
//		}
//	}
//	Destroy();
//
//	/*if (Role == ROLE_Authority)
//	{
//		MakeNoise(1, Instigator);
//		UE_LOG(LogTemp, Warning, TEXT("destroy!!!!---------->>>>"));
//	}*/
//}

void ASBulletProjectle::DestroyMe()
{
	if (ExplosionEmitter &&nullptr != sound)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEmitter, GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound, GetActorLocation());

		//apply damage
		FVector actorpos = GetActorLocation();
		//UGameplayStatics::ApplyRadialDamage(GetWorld(), 200.f, actorpos, 300.f,damageclass,ignoractors);
		DrawDebugSphere(GetWorld(), actorpos, 200.f, 12, FColor::Red,true,2,1,1);
	}
	Destroy();
}
