// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon.h"
#include "Granade.h"
#include "PomegranadeLauncher.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API APomeGranadeLauncher : public AWeapon
{
	GENERATED_BODY()

public:

	APomeGranadeLauncher(const FObjectInitializer& FOI);

	void PrimaryFunction(APlayerCharacter* user);
	void PrimaryFunctionReleased(APlayerCharacter* user);

	void SecondaryFunction(APlayerCharacter* user);

	void BeginPlay();

	void Fire();

	// Called every frame
	void Tick(float DeltaSeconds);
	void Reload() {};
	
	
};
