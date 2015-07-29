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

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	APomeGranadeLauncher(const FObjectInitializer& FOI);

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
