// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Sweapon.h"
#include "CoopGame.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Public/SHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	springarmcomp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springarmcomp"));
	springarmcomp->bUsePawnControlRotation = true;
	springarmcomp->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(Weaponchanel, ECR_Ignore);

	cameracomp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameracom"));
	cameracomp->SetupAttachment(springarmcomp);

	healthcomp = CreateDefaultSubobject<USHealthComponent>(TEXT("healthcomp"));
	healthcomp->OnhealthChanged.AddDynamic(this, &ASCharacter::OnhealthChanged);
	defaultFOV = 90.f;
	zoomedFOV = 60.f;
	WeaponSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	defaultFOV = cameracomp->FieldOfView;

	if (Role == ROLE_Authority)
	{
		FActorSpawnParameters parameters;
		parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		currentweapon = GetWorld()->SpawnActor<ASweapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, parameters);
		if (currentweapon)
		{
			currentweapon->SetOwner(this);
			currentweapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		}
	}
}

void ASCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector()*value);
}

void ASCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector()*value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::Fire()
{
	if (currentweapon)
	{
		currentweapon->StartFire();
	}
}

void ASCharacter::EndFire()
{
	if (currentweapon)
	{
		currentweapon->EndFire();
	}
}

void ASCharacter::BeginZoom()
{
	wantzoom = true;
}

void ASCharacter::EndZoom()
{
	wantzoom = false;
}

void ASCharacter::OnhealthChanged(USHealthComponent* helatcomp, float health, float dealtahealth, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (health <= 0.f && !bDie)
	{
		bDie = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();
		SetLifeSpan(10.f);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	currentFOV = wantzoom ? zoomedFOV : defaultFOV;

	float newfov = FMath::FInterpTo(cameracomp->FieldOfView, currentFOV, DeltaTime, zoominterpspeed);

	cameracomp->SetFieldOfView(newfov);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Move
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	//Look
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	//Crouch
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this,&ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	//Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	//Zoom
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	//Fire
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::EndFire);

}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (cameracomp)
	{
		return cameracomp->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

void ASCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASCharacter, currentweapon);
}