// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpectatorPawn.h"
#include "CMSpectator.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ACMSpectator : public ASpectatorPawn
{
	GENERATED_BODY()
	
public:

	ACMSpectator(const FObjectInitializer& ObjectInitializer);
	
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	void TryRespawn();
};
