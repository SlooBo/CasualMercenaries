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

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	/************************************************************************/
	/* Functionality                                                        */
	/************************************************************************/

	UFUNCTION()
	void OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	virtual void TakeDamage(float damage) override;


private:

	/************************************************************************/
	/* Hidden functionality                                                        */
	/************************************************************************/

	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void Explode();

	UFUNCTION(Reliable, NetMulticast, WithValidation)
		void Expand();

	// Called every second checking if actor should be replicated to target
	virtual bool IsNetRelevantFor(const AActor* realViewer, const AActor* viewTarget, const FVector& srcLocation) const;

	/************************************************************************/
	/* Values                                                               */
	/************************************************************************/

	UPROPERTY(Replicated)
	float health;

	UPROPERTY(Replicated)
	float size;

	/************************************************************************/
	/* Timers and triggers                                                  */
	/************************************************************************/

	UPROPERTY(Replicated)
	bool inflating;
	
	UPROPERTY(Replicated)
	bool isProjectile;

	
};
