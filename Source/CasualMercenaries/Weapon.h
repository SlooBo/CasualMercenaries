// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


class APlayerCharacter;

UENUM(BlueprintType)
enum class WEAPONID : uint8
{
	ROCKET_LAUNCHER,
	WATER_GUN,
	GRENADE_LAUNCHER,
	MASHINE_GUN,
	MUDBUSTER_GUN,
	F,
	G
};

UCLASS()
class CASUALMERCENARIES_API AWeapon : public AActor
{
	GENERATED_BODY()

public:	

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* weaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	// Sets default values for this actor's properties
	AWeapon(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;


	virtual void PrimaryFunction(APlayerCharacter* user);

	virtual void PrimaryFunctionReleased(APlayerCharacter* user){};

	virtual void SecondaryFunction(APlayerCharacter* user);

	virtual void SecondaryFunctionReleased(APlayerCharacter* user){};

	virtual void Reload();

	virtual void Fire(){};

	virtual void IncreaseAmmoAmount(int32 Ammo);

	void SetRoot(APlayerCharacter* user);
	
	WEAPONID GetID(){ return id; };

protected:

	WEAPONID id;

	int32 price;

	int ammo;
	int clips;
	int ammoInClip; 
	int maxAmmo;
	float firingInterval;

	USkeletalMesh* ammoMesh;

	float reloadTime;
	float passedTimeReloading;
	bool reloading;
	bool firing;
	float passedTimeFiring;

};
