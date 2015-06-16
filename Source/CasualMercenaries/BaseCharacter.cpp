// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "UnrealNetwork.h"
#include "BaseCharacter.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*health_Max = 100;
	health = health_Max;

	stamina_Max = 100;
	stamina = stamina_Max;*/

	/* Don't collide with camera checks to keep 3rd person camera at position when other players are standing behind player */
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseCharacter, health);
	DOREPLIFETIME(ABaseCharacter, stamina);
	DOREPLIFETIME(ABaseCharacter, armor);
}

float ABaseCharacter::GetHealth() const
{
	return health;
}

float ABaseCharacter::GetHealthMax() const
{
	return health_Max;
}

float ABaseCharacter::GetStamina() const
{
	return stamina;
}

float ABaseCharacter::GetStaminaMax() const
{
	return stamina_Max;
}

float ABaseCharacter::GetArmor() const
{
	return armor;
}

float ABaseCharacter::GetArmorMax() const
{
	return armor_Max;
}

bool ABaseCharacter::IsAlive() const
{
	return health > 0;
}

void ABaseCharacter::AddHealth(float _health)
{
	if (health + _health >= health_Max)
		health = health_Max;
	else
		health = health + _health;
}

void ABaseCharacter::TakeDamage(float _health)
{
	health = health - _health;
}

void ABaseCharacter::AddStamina(float _stamina)
{
	if (stamina + _stamina >= stamina_Max)
		stamina = stamina_Max;
	else
		stamina = stamina + _stamina;
}

void ABaseCharacter::LoseStamina(float _stamina)
{
	if (stamina - _stamina <= 0)
		stamina = 0;
	else
		stamina = stamina - _stamina;
}

void ABaseCharacter::AddArmor(float _armor)
{
	if (armor + _armor >= armor_Max)
		armor = armor_Max;
	else
		armor = armor + _armor;
}

void ABaseCharacter::LoseArmor(float _damage)
{
	if (armor - _damage <= 0)
		armor = 0;
	else
		armor = armor - _damage;
}

void ABaseCharacter::SetRagdollPhysics()
{
	bool tempInRagdoll = false;
	USkeletalMeshComponent* tempMesh = GetMesh();

	tempMesh->SetAllBodiesSimulatePhysics(true);
	tempMesh->SetSimulatePhysics(true);
	tempMesh->WakeAllRigidBodies();
	tempMesh->bBlendPhysics = true;
	tempInRagdoll = true;

	UCharacterMovementComponent* tempCharComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (tempCharComp)
	{
		tempCharComp->StopMovementImmediately();
		tempCharComp->DisableMovement();
		tempCharComp->SetComponentTickEnabled(false);
	}

	if (!tempInRagdoll)
	{
		//Immediately hide the pawn
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}

	SetLifeSpan(10.0f);
}