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
#include "Hook.h"
#include "SmokeGun.h"
void FInventory::ClearInventory()
{
	for (int i = 0; i < weapons.Num(); i++)
	{ 
		if (weapons[i] != nullptr && weapons.IsValidIndex(i))
		weapons[i]->Destroy();
	}  
	weapons.Reset();
	currentWeapon = 0;
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

AWeapon* FInventory::GetCurrentWeapon()
{
	return GetWeapon(currentWeapon);
}

void FInventory::ChangeWeaponAtSlot(uint16 slot, WEAPONID weaponid)
{
	if (weapons.Num() <= 0 || !weapons.IsValidIndex(slot))
	{
		return;
	}
	UClass *weaponClass= nullptr;
	AWeapon* weapon = nullptr;
	if (owningPlayer == nullptr || !owningPlayer->IsValidLowLevel())
		return;
	APlayerCharacter *playerCharacter = Cast<APlayerCharacter>(owningPlayer->GetPawn());
	if (playerCharacter == nullptr || !playerCharacter->IsValidLowLevel())
		return;
	switch (weaponid)
	{
		case WEAPONID::GRENADE_LAUNCHER:
		{
			weaponClass = APomeGranadeLauncher::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<APomeGranadeLauncher>(weaponClass, playerCharacter->GetActorLocation(), playerCharacter->GetActorRotation());
			break;
		}
		case WEAPONID::MASHINE_GUN:
		{
			weaponClass = AMashineGun::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<AMashineGun>(weaponClass, playerCharacter->GetActorLocation(), playerCharacter->GetActorRotation());
			break;
		}
		case WEAPONID::MUDBUSTER_GUN:
		{
			weaponClass = AMUDbuster::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<AMUDbuster>(weaponClass, playerCharacter->GetActorLocation(), playerCharacter->GetActorRotation());
			break;
		}
		case WEAPONID::ROCKET_LAUNCHER:
		{
			weaponClass = ARocketLauncher::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<ARocketLauncher>(weaponClass, playerCharacter->GetActorLocation(), playerCharacter->GetActorRotation());
			break;
		}
		case WEAPONID::WATER_GUN:
		{
			weaponClass = AUberWeihmacher::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<AUberWeihmacher>(weaponClass, playerCharacter->GetActorLocation(), playerCharacter->GetActorRotation());
			break;
		}
		case WEAPONID::WASP_GUN:
		{
			weaponClass = AWaspNestCudgel::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<AWaspNestCudgel>(weaponClass, playerCharacter->GetActorLocation(), playerCharacter->GetActorRotation());
			break;
		}
		case WEAPONID::TWISTER_GUN:
		{
			weaponClass = ATwisterSister::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<ATwisterSister>(weaponClass, playerCharacter->GetActorLocation(), playerCharacter->GetActorRotation());
			break;
		}
		case WEAPONID::SHOT_GUN:
		{
			weaponClass = AShotgun::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<AShotgun>(weaponClass, playerCharacter->GetActorLocation(), playerCharacter->GetActorRotation());
			break;
		}
		case WEAPONID::HOOK:
		{
			weaponClass = AHook::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<AHook>(weaponClass, playerCharacter->GetActorLocation(), playerCharacter->GetActorRotation());
			break;
		}
		case WEAPONID::SMOKE_GUN:
		{
			weaponClass = ASmokeGun::StaticClass();
			weapon = owningPlayer->GetWorld()->SpawnActor<ASmokeGun>(weaponClass, playerCharacter->GetActorLocation(), playerCharacter->GetActorRotation());
			break;
		}
		default:
		{
			return;
		}
	}

	weapon->SetRoot(playerCharacter);
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
void FInventory::SetPlayer(ACMPlayerController *player)
{
	owningPlayer = player;
}