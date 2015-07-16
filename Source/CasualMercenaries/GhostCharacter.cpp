// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMGameMode.h"
#include "GhostCharacter.h"

AGhostCharacter::AGhostCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// hide from other players
	bReplicates = false;
	bOnlyRelevantToOwner = true;

	UCharacterMovementComponent* characterMovement = GetCharacterMovement();
	//characterMovement->GravityScale *= 0.5f;
	characterMovement->JumpZVelocity *= 2;
	characterMovement->MaxWalkSpeed *= 2;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Spectator"));

	// ghost post-processing effects
	UCameraComponent* camera = GetCamera();
	camera->PostProcessSettings.bOverride_ColorSaturation = true;
	camera->PostProcessSettings.bOverride_ColorContrast = true;
	camera->PostProcessSettings.bOverride_VignetteIntensity = true;
	camera->PostProcessSettings.ColorSaturation.Set(0.0f, 0.0f, 0.0f);
	camera->PostProcessSettings.ColorContrast.Set(1.5f, 1.5f, 1.5f);
	camera->PostProcessSettings.VignetteIntensity = 0.76f;
}

void AGhostCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// use same input bindings as player character
	Super::SetupPlayerInputComponent(InputComponent);
}

void AGhostCharacter::BeginPlay()
{
	this->ChangeShirtColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

	// hide other players
	if (Role < ROLE_Authority)
	{
		for (TActorIterator<APlayerCharacter> iter(GetWorld()); iter; ++iter)
		{
			if (*iter != this && Cast<AGhostCharacter>(*iter) == NULL)
				(*iter)->SetActorHiddenInGame(true);
		}
	}
}

void AGhostCharacter::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	// reveal all players
	if (Role < ROLE_Authority)
	{
		for (TActorIterator<APlayerCharacter> iter(GetWorld()); iter; ++iter)
		{
			if (*iter != this && Cast<AGhostCharacter>(*iter) == NULL)
				(*iter)->SetActorHiddenInGame(false);
		}
	}

	Super::EndPlay(endPlayReason);
}

void AGhostCharacter::TakeDamage(float _damage, DAMAGE_TYPE _type, ACMPlayerController* killer)
{
}

void AGhostCharacter::FellOutOfWorld(const class UDamageType& DmgType)
{
	if (Role < ROLE_Authority)
		return;

	// reset ghost location

	ACMGameMode* gameMode = Cast<ACMGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	ACMPlayerController* playerController = Cast<ACMPlayerController>(GetController());

	if (gameMode != NULL && playerController != NULL)
		gameMode->SpectatePlayer(playerController);
}