// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.h"
#include "MudBall.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API AMudBall : public AProjectile
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, Category = Particles)
	UParticleSystemComponent* particleSystem;

	AMudBall(const FObjectInitializer& ObjectInitializer);

	~AMudBall();

	void Tick(float DeltaSeconds);
	void BeginPlay();

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void Explode();

	UFUNCTION()
		void OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);


private:

	float health;

	UParticleSystem* part;
	
};
