// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon.h"
#include "MUDbuster.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API AMUDbuster : public AWeapon
{
	GENERATED_BODY()

public:

	AMUDbuster(const FObjectInitializer& FOI);

	void PrimaryFunction(APlayerCharacter* user);

	void PrimaryFunctionReleased(APlayerCharacter* user);

	void SecondaryFunction(APlayerCharacter* user);

	void BeginPlay();

	void Reload();


	// Called every frame
	void Tick(float DeltaSeconds);




private:

	void Fire();
	
	
};
