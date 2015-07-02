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

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	AMudBall(const FObjectInitializer& ObjectInitializer);

	~AMudBall();

	void Tick(float DeltaSeconds);
	void BeginPlay();

	/************************************************************************/
	/* Functionality                                                        */
	/************************************************************************/

	UFUNCTION()
	void OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	void TakeDamage(float damage){ health -= damage; };


private:

	/************************************************************************/
	/* Hidden functionality                                                        */
	/************************************************************************/

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void Explode();


	/************************************************************************/
	/* Values                                                               */
	/************************************************************************/

	float health;
	float size;

	/************************************************************************/
	/* Timers and triggers                                                  */
	/************************************************************************/

	bool inflating;


	
};
