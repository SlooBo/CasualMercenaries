// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon.h"
#include "TwisterSister.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ATwisterSister : public AWeapon
{
	GENERATED_BODY()
	
public:

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	ATwisterSister(const FObjectInitializer& FOI);

	// Called every frame
	void Tick(float DeltaSeconds);

	void BeginPlay();

	/************************************************************************/
	/* Functionality                                                        */
	/************************************************************************/

	void PrimaryFunction(APlayerCharacter* user);

	void PrimaryFunctionReleased(APlayerCharacter* user);

	void SecondaryFunction(APlayerCharacter* user);

	void Reload();
	
private:

	/************************************************************************/
	/* Hidden Functionality                                                 */
	/************************************************************************/

	void Fire();

	/************************************************************************/
	/* ServerFunctions                                                      */
	/************************************************************************/

	//The effect that should play for everyone not only for the instigator 
	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void ServerEffect(UParticleSystem* particle, FVector location);
};
