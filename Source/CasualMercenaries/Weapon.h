// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DamageTypes.h"
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
	WASP_GUN,
	TWISTER_GUN,
	SHOT_GUN,
	HOOK,
	NO_WEAPON,
};

UCLASS()
class CASUALMERCENARIES_API AWeapon : public AActor
{
	GENERATED_BODY()

public:	

	/************************************************************************/
	/* UProperties                                                          */
	/************************************************************************/

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
	USkeletalMeshComponent* weaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector muzzleOffset;

	UPROPERTY(VisibleDefaultsOnly, Category = Particles)
	UParticleSystemComponent* particleSystem;

	UPROPERTY()
	UAudioComponent* audioComp;

	UFUNCTION(BlueprintImplementableEvent, Category = Animation)
	void NotifyFireStartJP();

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	// Sets default values for this actor's properties
	AWeapon(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/************************************************************************/
	/* Functionality                                                        */
	/************************************************************************/

	virtual void PrimaryFunction(APlayerCharacter* user);

	virtual void PrimaryFunctionReleased(APlayerCharacter* user){};

	virtual void SecondaryFunction(APlayerCharacter* user);

	virtual void SecondaryFunctionReleased(APlayerCharacter* user){};

	virtual void Reload();

	virtual void Fire(){};

	virtual void IncreaseAmmoAmount(int32 Ammo);

	/************************************************************************/
	/* Utility                                                              */
	/************************************************************************/

	void SetRoot(APlayerCharacter* user);
	
	WEAPONID GetID(){ return id; };
	static WEAPONID GetIDFromInt(uint8 value);

	void SuperFunctioAlaMiika();

	uint32 GetAmmo(){ return ammo; }

	bool GetReloading(){return reloading;};

	bool GetFiring(){return firing;};

	bool GetJustFired()
	{ 
		return passedTimeFiring == 0;
	};

	WEAPONID GetId(){ return id; };


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:

	/************************************************************************/
	/* Kept values                                                          */
	/************************************************************************/
	
	int damage;

	int clips;
	int ammoInClip; 
	int maxAmmo;
	int32 price;

	DAMAGE_TYPE type;
	WEAPONID id;
	UPROPERTY(Replicated)
	int32 ammo;

	/************************************************************************/
	/* Timers and triggers                                                  */
	/************************************************************************/

	float firingInterval;
	float reloadTime;
	float passedTimeReloading;
	float passedTimeFiring;

	bool reloading;
	bool firing;

	/************************************************************************/
	/* Particles                                                            */
	/************************************************************************/

	UParticleSystem* flavorParticleEffect;
	
	/************************************************************************/
	/* Sounds                                                               */
	/************************************************************************/

	UPROPERTY()
	TArray<USoundWave*> audioList;
};
