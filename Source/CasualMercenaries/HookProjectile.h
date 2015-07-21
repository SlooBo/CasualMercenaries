// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Projectile.h"
#include "HookProjectile.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API AHookProjectile : public AProjectile
{
	GENERATED_BODY()
public:

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	AHookProjectile(const FObjectInitializer& ObjectInitializer);

	~AHookProjectile();

	void Tick(float DeltaSeconds);
	void BeginPlay();

	/************************************************************************/
	/* Functionality                                                        */
	/************************************************************************/



	UFUNCTION()
		void OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);


	//float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	bool hit;
private:

	/************************************************************************/
	/* Values										                        */
	/************************************************************************/

	float lifeTime, livedTime;


	
	
	
};
