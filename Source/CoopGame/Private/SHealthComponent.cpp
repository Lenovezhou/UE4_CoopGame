// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"


// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	// ...
	defaulthealth = 100;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = defaulthealth;

	AActor* myowner = GetOwner();

	myowner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::TakeDamage);

	// ...
	
}

void USHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	Health -= Damage;
	Health = FMath::Max(Health, 0.f);
	OnhealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
	//UE_LOG(LogTemp, Warning, TEXT("Current Health == %s"),*FString::SanitizeFloat(Health));
}

