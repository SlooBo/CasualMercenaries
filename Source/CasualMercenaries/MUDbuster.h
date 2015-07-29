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

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	AMUDbuster(const FObjectInitializer& FOI);

	void BeginPlay();

	// Called every frame
	void Tick(float DeltaSeconds);

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
