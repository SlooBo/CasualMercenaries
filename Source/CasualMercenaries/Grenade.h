// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.h"
#include "Grenade.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API AGrenade : public AProjectile
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = Particles)
		UParticleSystemComponent* particleSystem;

	AGrenade(const FObjectInitializer& ObjectInitializer);

	~AGrenade();

	void Tick(float DeltaSeconds);
	void BeginPlay();
	
	
};
