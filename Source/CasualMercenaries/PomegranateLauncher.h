// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon.h"
#include "PomegranateLauncher.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API APomegranateLauncher : public AWeapon
{
	GENERATED_BODY()

public:

	APomegranateLauncher(const FObjectInitializer& FOI);

	void PrimaryFunction(APlayerCharacter* user);

	void SecondaryFunction();

	void BeginPlay();

	// Called every frame
	void Tick(float DeltaSeconds);
	void Reload() {};
	
	
};
