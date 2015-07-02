// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.h"
#include "Twister.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ATwister : public AProjectile
{
	GENERATED_BODY()
	
public:

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	ATwister(const FObjectInitializer& ObjectInitializer);

	~ATwister(){};

	void Tick(float DeltaSeconds);

	void BeginPlay();

	/************************************************************************/
	/* Functionality                                                        */
	/************************************************************************/

	UFUNCTION()
		void OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		virtual void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	/************************************************************************/
	/* Hidden functionality                                                        */
	/************************************************************************/

	void Explode();
	
	
};
