// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Projectile.h"
#include "Rocket.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ARocket : public AProjectile
{

	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Particles)
		TSubobjectPtr<class UParticleSystemComponent> particleSystem;

	ARocket(const FObjectInitializer& ObjectInitializer);

	~ARocket();
	
	void Tick(float DeltaSeconds);
	void BeginPlay();


protected:



private:


	
};
