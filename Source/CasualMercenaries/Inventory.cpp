// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"

#include "Inventory.h"

UInventory::UInventory()
{
}

UInventory::~UInventory()
{
}

void UInventory::AddWeaponToInventory(AWeapon* weapon)
{
	weapons.Add(weapon);
}

void UInventory::RemoveWeaponFromInventpry(int number)
{

	weapons.RemoveAt(number, 1);
}

void UInventory::RemoveWeaponFromInventory(AWeapon* weapon)
{
	for (int i = 0; i < weapons.Num(); i++)
	{
		if (weapon == weapons[i])
		{
			weapons.RemoveAt(i, 1);;
		}
	}
}

AWeapon* UInventory::GetWeapon(int number)
{
	if (weapons.Num() == 0)
		return nullptr;
	return weapons[number];
}



