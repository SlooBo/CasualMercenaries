// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "UnrealNetwork.h"
#include "BaseCharacter.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	health_Max = 100;
	health = health_Max;

	stamina_Max = 100;
	stamina = stamina_Max;

	armor_Max = 100;
	armor = 100;

	state = CHARACTER_STATE::ALIVE;

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
	DOREPLIFETIME(ABaseCharacter, state);
}

void ABaseCharacter::FellOutOfWorld(const class UDamageType& DmgType)
{
	health = 0;
	OnDeath();
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

CHARACTER_STATE ABaseCharacter::GetState() const
{
	return state;
}

bool ABaseCharacter::IsAlive() const
{
	return health >= 1;
}

void ABaseCharacter::RestoreStats()
{
	health = health_Max;
	armor = armor_Max;
	stamina = stamina_Max;
}

void ABaseCharacter::AddHealth(float _health)
{
	if (health + _health >= health_Max)
		health = health_Max;
	else
		health = health + _health;
}

void ABaseCharacter::TakeDamage(float _health, DAMAGE_TYPE _type, ACMPlayerController* _damageSource)
{
	if (!IsAlive() || GetController() == NULL || Role < ROLE_AutonomousProxy) //skip if not owner
		return;

	health = health - _health;
	
	switch (_type)
	{
	case DAMAGE_TYPE::NORMAL:
		break;
	case DAMAGE_TYPE::STUN:
		SetState(CHARACTER_STATE::STUNNED);
		break;
	case DAMAGE_TYPE::ROOT:
		SetState(CHARACTER_STATE::ROOTED);
		break;
	default:
		break;
	}

	//Temp fix
	if (!IsAlive())
		OnDeath(_damageSource);
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

void ABaseCharacter::SetState(CHARACTER_STATE _state)
{
	state = _state;
}

void ABaseCharacter::OnDeath_Implementation(ACMPlayerController* damageSource)
{
	// delay the destruction until the player controller no longer controls this character
	// if pawn is destroyed before the controller acknowledges it, crash happens

	DelayedDestroy();
}

void ABaseCharacter::DelayedDestroy()
{
	FTimerHandle timerHandle;
	FTimerDelegate destroyDelegate = FTimerDelegate::CreateUObject<ABaseCharacter, FTimerHandle&>(this, &ABaseCharacter::DelayedDestroy, timerHandle);
	GetWorld()->GetTimerManager().SetTimer(timerHandle, destroyDelegate, 1.0f, true);

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void ABaseCharacter::DelayedDestroy(FTimerHandle& timerHandle)
{
	if (Controller == NULL)
	{
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		Destroy(true);
	}
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