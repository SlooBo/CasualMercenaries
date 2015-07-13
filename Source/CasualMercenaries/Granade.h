// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.h"
#include "Granade.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API AGranade : public AProjectile
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	AGranade(const FObjectInitializer& ObjectInitializer);

	~AGranade();

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
};
