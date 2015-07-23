// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "DestructibleObject.h"
#include "UnrealNetwork.h"

// Sets default values
ADestructibleObject::ADestructibleObject(const FObjectInitializer& FOI) : AActor(FOI)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	healthMax = 100;
	health = healthMax;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Props/Firehydrant/firehydrant.firehydrant'"));
	
	meshComponent = FOI.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMeshComponent"));
	if (MeshObj.Object != NULL)
	{
		meshComponent->SetStaticMesh(MeshObj.Object);
	}

	timerActive = false;
}

// Called when the game starts or when spawned
void ADestructibleObject::BeginPlay()
{
	Super::BeginPlay();
	//health = 0;

}

// Called every frame
void ADestructibleObject::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	CheckStatus();
}

void ADestructibleObject::EndPlay(const EEndPlayReason::Type _endPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void ADestructibleObject::TakeDamage(float _damage)
{
	if (!timerActive)
	{
		health = health - _damage;
	}
}

void ADestructibleObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Value is already updated locally, skip in replication step
	//DOREPLIFETIME_CONDITION(APlayerCharacter, value, COND_SkipOwner);

	//Replicated to every client, no special condition required
	//DOREPLIFETIME(APlayerCharacter, value);
	DOREPLIFETIME(ADestructibleObject, health);
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