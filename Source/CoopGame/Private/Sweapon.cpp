// Fill out your copyright notice in the Description page of Project Settings.

#include "Sweapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "CoopGame.h"	
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Net/UnrealNetwork.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDeabugDrawing(TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing, 
	TEXT("Draw Debug line"),
	ECVF_Cheat);


// Sets default values
ASweapon::ASweapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	meshcomponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("skeletalmesh"));
	RootComponent = meshcomponent;
	MuzzelSocketName = "MuzzleSocket";
	TracerEffectTargetName = "BeamEnd";
	basedamage = 20.f;
	SetReplicates(true);

	NetUpdateFrequency = 60.f;
	MinNetUpdateFrequency = 33.f;
}


void ASweapon::BeginPlay()
{
	firerate = 60 / firepreminitNUM;
}



void ASweapon::Fire()
{

	if (Role< ROLE_Authority)
	{
		ServerFire();
	}

	AActor* myowner = GetOwner();

	if (myowner)
	{
		FVector eyelocaltion;
		FRotator eyerotation;
		myowner->GetActorEyesViewPoint(eyelocaltion, eyerotation);

		FVector hitdirection = eyerotation.Vector();

		FVector traceend = eyelocaltion + hitdirection * 10000;

		FVector TrancerEndPoint = traceend;

		FCollisionQueryParams queryparams;
		queryparams.AddIgnoredActor(myowner);
		queryparams.AddIgnoredActor(this);
		queryparams.bTraceComplex = true;
		queryparams.bReturnPhysicalMaterial = true;

		float currentdamage = basedamage;
		EPhysicalSurface hitsurface = SurfaceType_Default;
		FHitResult hit;
		if (GetWorld()->LineTraceSingleByChannel(hit, eyelocaltion, traceend, Weaponchanel, queryparams))
		{
			TrancerEndPoint = hit.ImpactPoint;
			hitsurface = UPhysicalMaterial::DetermineSurfaceType(hit.PhysMaterial.Get());
			// apply damage
			if (hitsurface == FleshVulnerableSuface)
			{
				currentdamage = basedamage *= 4;
			}
			UGameplayStatics::ApplyPointDamage(hit.GetActor(), currentdamage, hitdirection, hit, myowner->GetInstigatorController(), this, damagetypeclass);
			
			PlayImpactEffect(hitsurface, TrancerEndPoint);
		}
		lastfiretime = GetWorld()->TimeSeconds;
		PlayFireEffect(TrancerEndPoint);
		HitScanTrace.surfacetype = hitsurface;
		if (Role == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TrancerEndPoint;
		}

	}

}

void ASweapon::OnRep_HitScanTrace()
{
	PlayFireEffect(HitScanTrace.TraceTo);
	PlayImpactEffect(HitScanTrace.surfacetype, HitScanTrace.TraceTo);
}

void ASweapon::ServerFire_Implementation()
{
	Fire();
}

bool ASweapon::ServerFire_Validate()
{
	return true;
}


void ASweapon::StartFire()
{
	float firstdelay = FMath::Max((lastfiretime + firerate - GetWorld()->TimeSeconds),0.f);
	GetWorldTimerManager().SetTimer(fireratehandle,this,&ASweapon::Fire, firerate, true, firstdelay);
}

void ASweapon::EndFire()
{
	GetWorldTimerManager().ClearTimer(fireratehandle);
}

void ASweapon::PlayFireEffect(FVector TrancerEndPos)
{
	if (MuzzelEffect)
	{
		//UGameplayStatics::SpawnEmitterAttached(MuzzelEffect, meshcomponent, MuzzelSocketName);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzelEffect, meshcomponent->GetSocketLocation(MuzzelSocketName));
	}
	if (TracerEffect)
	{
		FVector TracerSpawnPos = meshcomponent->GetSocketLocation(MuzzelSocketName);
		UParticleSystemComponent* tracercomp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, TracerSpawnPos);
		tracercomp->SetVectorParameter(TracerEffectTargetName, TrancerEndPos);
	}

	if (DebugWeaponDrawing > 0)
	{
		DrawDebugSphere(GetWorld(), TrancerEndPos, 50, 12, FColor::Black, false, 1, 0, 1);
		UE_LOG(LogTemp, Warning, TEXT("can draw"));
	}

	//CameraShake
	APawn* myowner = Cast<APawn>(GetOwner());
	if (myowner)
	{
		APlayerController* pc = Cast<APlayerController>(myowner->GetController());
		if (pc)
		{
			pc->ClientPlayCameraShake(camerashakeanim);
		}
	}
}

void ASweapon::PlayImpactEffect(EPhysicalSurface surfacetype,FVector ImpactPoint)
{
	UParticleSystem* currentparticle = nullptr;
	switch (surfacetype)
	{
		case DefaultFleshSurface:
		case FleshVulnerableSuface:
			currentparticle = FleshVulnerableEffect;
			break;
		default:
			currentparticle = DefaultEffect;
			break;
	}

	if (currentparticle)
	{
		FVector MuzzelSocketLocation = meshcomponent->GetSocketLocation(MuzzelSocketName);
		FVector ShotDirection = (ImpactPoint - MuzzelSocketLocation);
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), currentparticle, ImpactPoint, ShotDirection.Rotation());
	}
}

void ASweapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ASweapon, HitScanTrace, COND_SkipOwner);
}