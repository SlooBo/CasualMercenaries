// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.h"
#include "DustBag.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ADustBag : public AProjectile
{
	GENERATED_BODY()

public:
	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	ADustBag(const FObjectInitializer& ObjectInitializer);

	~ADustBag();

	void Tick(float DeltaSeconds);
	void BeginPlay();

	/************************************************************************/
	/* Functionality                                                        */
	/************************************************************************/


	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void Explode();

	UFUNCTION()
		void OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);


	//float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


private:

	/************************************************************************/
	/* Values										                        */
	/************************************************************************/

	float lifeTime, livedTime;

	URadialForceComponent *radialForceComponent;
	
	
};
