// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API AShotgun : public AWeapon
{
	GENERATED_BODY()
	
	
	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	AShotgun(const FObjectInitializer& FOI);

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
	/* Hidden Functionality                                                 */
	/************************************************************************/

	void Fire();

	void DrawLine(FVector begin, FVector end);


	/************************************************************************/
	/* ServerFunctions                                                      */
	/************************************************************************/

	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void ServerDrawLine(FVector begin, FVector end);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void ServerEffect(UParticleSystem* particle, FVector location);

	/************************************************************************/
	/* Particles		                                                    */
	/************************************************************************/

	UParticleSystem* bulletTrail;
	
};
