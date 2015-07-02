// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "Rocket.h"
#include "RocketLauncher.generated.h"

UCLASS()
class CASUALMERCENARIES_API ARocketLauncher : public AWeapon
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	ARocketLauncher(const FObjectInitializer& FOI);

	void BeginPlay();

	// Called every frame
	void Tick(float DeltaSeconds);

	/************************************************************************/
	/* Functionality                                                        */
	/************************************************************************/

	void PrimaryFunction(APlayerCharacter* user);

	void PrimaryFunctionReleased(APlayerCharacter* user);

	void SecondaryFunction(APlayerCharacter* user);

	void Reload();

private:

	/************************************************************************/
	/* Hidden functionality                                                 */
	/************************************************************************/

	void Fire();

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ServerEffect(UParticleSystem* particle, FVector location);
};
