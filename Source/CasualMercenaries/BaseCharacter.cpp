// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
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
//
//float ABaseCharacter::GetHealth() const
//{
//	return health;
//}
//
//bool ABaseCharacter::IsAlive() const
//{
//	return health > 0;
//}
//
//void ABaseCharacter::SetRagdollPhysics()
//{
//	//bool tempInRagdoll = false;
//	//USkeletalMeshComponent* tempMesh = GetMesh();
//
//	//tempMesh->SetAllBodiesSimulatePhysics(true);
//	//tempMesh->SetSimulatePhysics(true);
//	//tempMesh->WakeAllRigidBodies();
//	//tempMesh->bBlendPhysics = true;
//	//tempInRagdoll = true;
//
//	//UCharacterMovementComponent* tempCharComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
//	//if (tempCharComp)
//	//{
//	//	tempCharComp->StopMovementImmediately();
//	//	tempCharComp->DisableMovement();
//	//	tempCharComp->SetComponentTickEnabled(false);
//	//}
//
//	//if (!tempInRagdoll)
//	//{
//	//	//Immediately hide the pawn
//	//	TurnOff();
//	//	SetActorHiddenInGame(true);
//	//	SetLifeSpan(1.0f);
//	//}
//
//	//SetLifeSpan(10.0f);
//}