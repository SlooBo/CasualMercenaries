// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DamageTypes.h"
#include "CMPlayerController.h"


#include "Projectile.generated.h"

#pragma warning(disable : 4263) // C4263: member function does not override any base class virtual member function
#pragma warning(disable : 4264) // C4264: no override available for virtual member function from base 'class'; function is hidden

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

	UPROPERTY()
		UAudioComponent* audioComp;

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	// Sets default values for this actor's properties
	AProjectile(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every second checking if actor should be replicated to target
	virtual bool IsNetRelevantFor(const AActor* realViewer, const AActor* viewTarget, const FVector& srcLocation) const;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/************************************************************************/
	/* Functionality                                                        */
	/************************************************************************/

	void InitVelocity(const FVector& ShootDirection);

	virtual void TakeDamage(float damage){};

	void SetController(ACMPlayerController *controller){ this->controller = controller; };


protected:

	/************************************************************************/
	/* Values                                                               */
	/************************************************************************/

	DAMAGE_TYPE type;

	float lifeTime;
	float deathTime;

	/************************************************************************/
	/* Particles                                                            */
	/************************************************************************/

	UParticleSystem* flavorParticleEffect;

	/************************************************************************/
	/* Sounds                                                               */
	/************************************************************************/

	UPROPERTY()
		TArray<USoundWave*> audioList;

	ACMPlayerController *controller;

};
