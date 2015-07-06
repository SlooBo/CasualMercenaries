// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"

#include "Inventory.h"
#include "UnrealNetwork.h"
#include "PlayerCharacter.h"
#include "MUDbuster.h"
#include "PomegranadeLauncher.h"
#include "RocketLauncher.h"
#include "MashineGun.h"
#include "UberWeihmacher.h"
#include "TwisterSister.h"
#include "WaspNestCudgel.h"
#include "Shotgun.h"
void FInventory::ClearInventory()
{
	for (int i = 0; i < weapons.Num(); i++)
	{ 
		if (weapons[i] != nullptr && weapons.IsValidIndex(i))
		weapons[i]->Destroy();
	}  
	weapons.Reset();
}

void FInventory::AddWeaponToInventory(AWeapon* weapon)
{
	weapons.Add(weapon);
}

void FInventory::RemoveWeaponFromInventory(int number)
{
	weapons.RemoveAt(number, 1);
}

void FInventory::RemoveWeaponFromInventory(AWeapon* weapon)
{
	for (int i = 0; i < weapons.Num(); i++)
	{
		if (weapon == weapons[i])
		{
			weapons.RemoveAt(i, 1);;
		}
	}
}

AWeapon* FInventory::GetWeapon(int number)
{
	if (!weapons.IsValidIndex(number))
		return nullptr;
	return weapons[number];
}
void FInventory::ChangeWeaponAtSlot(uint16 slot, WEAPONID weaponid)
{
	if (weapons.Num() <= 0 || !weapons.IsValidIndex(slot))
	{
		return;
	}
	UClass *weaponClass= nullptr;
	AWeapon* weapon = nullptr;
	switch (weaponid)
	{
		case WEAPONID::GRENADE_LAUNCHER:
		{
			weaponClass = APomeGranadeLauncher::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<APomeGranadeLauncher>(weaponClass, owningPlayer->GetActorLocation(), owningPlayer->GetActorRotation());
			break;
		}
		case WEAPONID::MASHINE_GUN:
		{
			weaponClass = AMashineGun::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<AMashineGun>(weaponClass, owningPlayer->GetActorLocation(), owningPlayer->GetActorRotation());
			break;
		}
		case WEAPONID::MUDBUSTER_GUN:
		{
			weaponClass = AMUDbuster::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<AMUDbuster>(weaponClass, owningPlayer->GetActorLocation(), owningPlayer->GetActorRotation());
			break;
		}
		case WEAPONID::ROCKET_LAUNCHER:
		{
			weaponClass = ARocketLauncher::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<ARocketLauncher>(weaponClass, owningPlayer->GetActorLocation(), owningPlayer->GetActorRotation());
			break;
		}
		case WEAPONID::WATER_GUN:
		{
			weaponClass = AUberWeihmacher::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<AUberWeihmacher>(weaponClass, owningPlayer->GetActorLocation(), owningPlayer->GetActorRotation());
			break;
		}
		case WEAPONID::WASP_GUN:
		{
			weaponClass = AWaspNestCudgel::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<AWaspNestCudgel>(weaponClass, owningPlayer->GetActorLocation(), owningPlayer->GetActorRotation());
			break;
		}
		case WEAPONID::TWISTER_GUN:
		{
			weaponClass = ATwisterSister::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<ATwisterSister>(weaponClass, owningPlayer->GetActorLocation(), owningPlayer->GetActorRotation());
			break;
		}
		case WEAPONID::SHOT_GUN:
		{
			weaponClass = AShotgun::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<AShotgun>(weaponClass, owningPlayer->GetActorLocation(), owningPlayer->GetActorRotation());
			break;
		}
		default:
		{
			return;
		}
	}

	weapon->SetRoot(owningPlayer);
	ChangeWeaponAtSlot(slot, weapon);
}
void FInventory::ChangeWeaponAtSlot(uint16 slot, AWeapon *newWeapon)
{
	if (weapons.Num() > 0 && weapons.IsValidIndex(slot))
	{
		if (weapons[slot] != nullptr)
		weapons[slot]->Destroy();
		weapons[slot] = newWeapon;
	}
}
void FInventory::SetPlayer(APlayerCharacter *player)
{
	owningPlayer = player;
}