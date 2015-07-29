// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "MashineGun.generated.h"
/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API AMashineGun : public AWeapon
{

	GENERATED_BODY()

public:

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	AMashineGun(const FObjectInitializer& FOI);

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
