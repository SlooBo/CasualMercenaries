// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMPlayerController.h"
#include "PlayerCharacter.h"

ACMPlayerController::ACMPlayerController(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{

}

void ACMPlayerController::BeginPlay()
{

}

void ACMPlayerController::OnPlayerDeath(ACMPlayerController* killed, ACMPlayerController* killer/*, AWeapon* weapon*/)
{
	if (killed == this)
	{
		APlayerCharacter* playerCharacter = static_cast<APlayerCharacter*>(GetPawn());
		if (playerCharacter != NULL)
		{
			UnPossess();
		}
	}
}