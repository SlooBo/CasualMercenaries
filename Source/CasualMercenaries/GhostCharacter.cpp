// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "GhostCharacter.h"

AGhostCharacter::AGhostCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//hide from other players
	bReplicates = false;
	bOnlyRelevantToOwner = true;

	UCharacterMovementComponent* characterMovement = GetCharacterMovement();
	//characterMovement->GravityScale *= 0.5f;
	characterMovement->JumpZVelocity *= 2;
	characterMovement->MaxWalkSpeed *= 2;
}

void AGhostCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	for (int i = 0; i < InputComponent->GetNumActionBindings(); i++)
	{
		FInputActionBinding binding = InputComponent->GetActionBinding(i);
		FString actionString = binding.ActionName.ToString();

		if (actionString.StartsWith("WeaponSlot") || actionString.StartsWith("MouseWheel")
			|| actionString == "LeftMouseButton" || actionString == "RightMouseButton" || actionString == "Reload")
		{
			InputComponent->RemoveActionBinding(i--);
		}
	}

	InputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &AGhostCharacter::TryRespawn);
}

void AGhostCharacter::BeginPlay()
{
	this->ChangeShirtColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
}

void AGhostCharacter::TryRespawn()
{
	ACMPlayerController* playerController = Cast<ACMPlayerController>(GetController());
	if (playerController != NULL)
		playerController->RequestRespawn();
}

void AGhostCharacter::TakeDamage(float _damage, DAMAGE_TYPE _type, ACMPlayerController* killer)
{
}