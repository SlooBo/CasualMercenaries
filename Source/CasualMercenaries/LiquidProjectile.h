// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.h"
#include "LiquidProjectile.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ALiquidProjectile : public AProjectile
{
	GENERATED_BODY()
		void makeGame();
	
public:

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	ALiquidProjectile(const FObjectInitializer& ObjectInitializer);

	~ALiquidProjectile();

	void Tick(float DeltaSeconds);
	void BeginPlay();

	/************************************************************************/
	/* Functionality                                                        */
	/************************************************************************/

	UFUNCTION()
		void OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);


private:

	/************************************************************************/
	/* Hidden functionality                                                        */
	/************************************************************************/

	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void Splash();
	
};
