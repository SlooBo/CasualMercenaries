// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Weapon.h"
#include "WeaponData.h"
#include "PlayerCharacter.h"
#include "UnrealNetwork.h"
#include "CMPlayerController.h"

// Sets default values
AWeapon::AWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//Initialize weaponMesh
	weaponMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponMesh"));
	PrimaryActorTick.bCanEverTick = true;


	//Hides Weapons unless called from code
	this->SetActorHiddenInGame(true);


	//default price
	price = 5;


	//trigger initial values
	reloading = false;
	firing = false;

	passedTimeFiring = 1;


	//Replicates itself overserver
	bReplicates = true;
}

// Function that sets values from WeaponData file to weapons 
// should be called after id has been set.
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

	// reloading can start only on tick
	// may induce unwanted consequenses

	passedTimeFiring += DeltaTime;

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
		else
		{
			return;
		}
	}



	// firing can start only on tick
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
// Old method that was used to child weapons to PlayerCharacter parents
void AWeapon::SetRoot(APlayerCharacter* user)
{
	this->SetOwner(user->GetController());
	this->AttachRootComponentToActor(user);
	controller = Cast<ACMPlayerController>(user->GetController());
}
// Gets you the weapon ID
WEAPONID AWeapon::GetIDFromInt(uint8 value)
{
	return static_cast<WEAPONID>(value);
}

// Function to preserve weapons from carbage collecting
// may be unnesessary now
void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, ammo);
}