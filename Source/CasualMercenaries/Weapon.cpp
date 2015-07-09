// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Weapon.h"
#include "WeaponData.h"
#include "PlayerCharacter.h"


// Sets default values
AWeapon::AWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	weaponMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh"));

	PrimaryActorTick.bCanEverTick = true;

	this->SetActorHiddenInGame(true);

	price = 5;

	reloading = false;
	firing = false;

	bReplicates = true;

	passedTimeFiring = 1;
	
	

}

void AWeapon::SuperFunctioAlaMiika()
{
	FWeaponStruct* data	= WeaponData::Get()->GetWeaponData(id);
	price = data->buyPrice;
	ammoInClip = data->clipSize;
	damage = data->damage;
	firingInterval = data->fireRate;
	reloadTime = data->reloadTime;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapon::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);
	if (reloading)
	{
		passedTimeReloading += DeltaTime;
		if (passedTimeReloading >= reloadTime)
		{
			ammo = ammoInClip;
			clips--;
			passedTimeReloading = 0;
			reloading = false;
		}
	}

	passedTimeFiring += DeltaTime;
	if (firing)
	{
		if (passedTimeFiring > firingInterval)
		{
			if (ammo < 1)
			{
				firing = false;
				return;
			}
			Fire();
			passedTimeFiring = 0;
		}
	}
}

void AWeapon::PrimaryFunction(APlayerCharacter* user)
{


}

void AWeapon::SecondaryFunction(APlayerCharacter* user)
{

}

void AWeapon::Reload()
{

}

void AWeapon::SetRoot(APlayerCharacter* user)
{
	this->SetOwner(user);
	this->AttachRootComponentToActor(user);
}

void AWeapon::IncreaseAmmoAmount(int32 ammo)
{
	clips += ammo;
}
WEAPONID AWeapon::GetIDFromInt(uint8 value)
{
	return static_cast<WEAPONID>(value);
}