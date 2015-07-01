// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon.h"
#include "WaspNestCudgel.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API AWaspNestCudgel : public AWeapon
{
	GENERATED_BODY()

public:

	AWaspNestCudgel(const FObjectInitializer& FOI);

	void PrimaryFunction(APlayerCharacter* user);

	void PrimaryFunctionReleased(APlayerCharacter* user);

	void SecondaryFunction(APlayerCharacter* user);

	void BeginPlay();

	void Reload();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Particles)
		UParticleSystemComponent* particleSystem;


	// Called every frame
	void Tick(float DeltaSeconds);




private:

	void Fire();


	
	
	
};
