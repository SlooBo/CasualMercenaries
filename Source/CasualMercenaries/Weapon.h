// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Weapon.generated.h"



UCLASS()
class CASUALMERCENARIES_API AWeapon : public AActor
{
	GENERATED_BODY()

public:	

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* weaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AProjectile> ProjectileClass;

	// Sets default values for this actor's properties
	AWeapon();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void PrimaryFunction(AActor user);

	virtual void SecondaryFunction();

	virtual void Reload();

protected:

	int ammo;
	int clips;
	int ammoInClip;
	int maxAmmo;

	USkeletalMesh* ammoMesh;

	float reloadTime;

};
