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

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	ARocket(const FObjectInitializer& ObjectInitializer);

	~ARocket();
	
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
	/* Hidden functionality                                                 */
	/************************************************************************/

	void Explode();

	/************************************************************************/
	/* Values				                                                */
	/************************************************************************/

	URadialForceComponent *radialForceComponent;

	bool directHitPlayer;
	bool casd;
	float asd;

};
