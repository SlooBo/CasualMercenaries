// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon.h"
#include "SmokeGun.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ASmokeGun : public AWeapon
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	ASmokeGun(const FObjectInitializer& FOI);

	// Called every frame
	void Tick(float DeltaSeconds);

	void BeginPlay();

	/************************************************************************/
	/* Functionality                                                        */
	/************************************************************************/

	void PrimaryFunction(APlayerCharacter* user);

	void PrimaryFunctionReleased(APlayerCharacter* user);

	void SecondaryFunction(APlayerCharacter* user);

	virtual void Reload(bool instantReload = false) override;

private:

	/************************************************************************/
	/* Hidden functionality                                                 */
	/************************************************************************/

	void Fire();

	
	
};
