// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DamageTypes.h"
#include "Projectile.generated.h"

UCLASS()
class CASUALMERCENARIES_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	/************************************************************************/
	/* UProperties                                                          */
	/************************************************************************/

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UStaticMeshComponent* projectileMesh;

	UPROPERTY(VisibleAnywhere, Category = Particles)
		UParticleSystemComponent* particleSystem;

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	// Sets default values for this actor's properties
	AProjectile(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/************************************************************************/
	/* Functionality                                                        */
	/************************************************************************/

	void InitVelocity(const FVector& ShootDirection);

	virtual void TakeDamage(float damage){};


protected:

	/************************************************************************/
	/* Values                                                               */
	/************************************************************************/

	DAMAGE_TYPE type;

	/************************************************************************/
	/* Particles                                                            */
	/************************************************************************/

	UParticleSystem* flavorParticleEffect;

};
