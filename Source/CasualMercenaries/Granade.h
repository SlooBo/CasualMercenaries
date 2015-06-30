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


	UPROPERTY(VisibleAnywhere, Category = Particles)
		UParticleSystemComponent* particleSystem;

	//float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	AGranade(const FObjectInitializer& ObjectInitializer);

	~AGranade();

	void Tick(float DeltaSeconds);
	void BeginPlay();


	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void Explode();


private:

	float lifeTime, livedTime;

	UParticleSystem* part;
	
	
};
