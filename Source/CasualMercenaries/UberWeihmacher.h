// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "UberWeihmacher.generated.h"

UCLASS()
class CASUALMERCENARIES_API AUberWeihmacher : public AWeapon
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Particles)
		UParticleSystemComponent* particleSystem;


	//väliaikanen
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UStaticMeshComponent* Mesh;

	// Sets default values for this actor's properties
	AUberWeihmacher(const FObjectInitializer& FOI);

	// Called when the game starts or when spawned
	void BeginPlay();

	void Fire(){};
	
	// Called every frame
	void Tick( float DeltaSeconds );

	void PrimaryFunction(APlayerCharacter* user);

	void PrimaryFunctionReleased(APlayerCharacter* user);

	void SecondaryFunction(APlayerCharacter* user);

private:

	float burntime;
	float pressure;
};
