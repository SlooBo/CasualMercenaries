// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMSpectator.h"
#include "CMPlayerController.h"

ACMSpectator::ACMSpectator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void ACMSpectator::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &ACMSpectator::TryRespawn);
	InputComponent->BindAction("Reload", IE_Pressed, this, &ACMSpectator::TryRespawn);
}

void ACMSpectator::TryRespawn()
{
	ACMPlayerController* playerController = Cast<ACMPlayerController>(GetController());
	if (playerController != NULL)
		playerController->RequestRespawn();
}