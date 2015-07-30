// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "DestructibleObject.h"
#include "UnrealNetwork.h"

// Sets default values
ADestructibleObject::ADestructibleObject(const FObjectInitializer& FOI) : AActor(FOI)
{
	healthMax = 100;
	health = healthMax;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Props/Firehydrant/firehydrant.firehydrant'"));
	
	meshComponent = FOI.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMeshComponent"));
	if (MeshObj.Object != NULL)
	{
		meshComponent->SetStaticMesh(MeshObj.Object);
	}

	timerActive = false;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ADestructibleObject::BeginPlay()
{
	Super::BeginPlay();
}

void ADestructibleObject::EndPlay(const EEndPlayReason::Type _endPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void ADestructibleObject::TakeDamage(float _damage)
{
	if (Role < ROLE_Authority)
		return;

	if (!timerActive)
	{
		health = health - _damage;
		CheckStatus();
	}
}

void ADestructibleObject::CheckStatus()
{
	if (health <= 0)
	{
		GoInvisible();
		health = healthMax;
		GetWorld()->GetTimerManager().SetTimer(respawnTimerHandle, this, &ADestructibleObject::Respawn, 20, false);
		timerActive = true;
	}
}

void ADestructibleObject::Respawn_Implementation()
{
	meshComponent->SetMaterial(0, normalMaterial);
	GetWorld()->GetTimerManager().ClearTimer(respawnTimerHandle);
	timerActive = false;
	SetActorEnableCollision(true);
}

void ADestructibleObject::GoInvisible_Implementation()
{
	meshComponent->SetMaterial(0, invisibleMaterial);
	SetActorEnableCollision(false);
}