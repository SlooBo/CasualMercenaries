// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Weapon.h"
#include "PlayerCharacter.h"


// Sets default values
AWeapon::AWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

	weaponMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("RocketLauncherMesh"));
	PrimaryActorTick.bCanEverTick = true;

	this->SetActorHiddenInGame(true);

	reloading = false;

	bReplicates = true;

	price = 5;
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