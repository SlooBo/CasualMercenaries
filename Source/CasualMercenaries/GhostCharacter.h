// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerCharacter.h"
#include "GhostCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API AGhostCharacter : public APlayerCharacter
{
	GENERATED_BODY()
	
public:
	AGhostCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	virtual void TakeDamage(float _damage, DAMAGE_TYPE _type, ACMPlayerController* killer = NULL);
};
