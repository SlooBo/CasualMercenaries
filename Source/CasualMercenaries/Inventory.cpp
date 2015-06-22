// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Inventory.h"

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
}

void Inventory::AddWeaponToInventory(AWeapon* weapon)
{
	weapons.push_back(weapon);
}

void Inventory::RemoveWeaponFromInventpry(int number)
{
	
	weapons.erase(weapons.begin() + number);
}

void Inventory::RemoveWeaponFromInventory(AWeapon* weapon)
{
	for (int i = 0; i < weapons.size(); i++)
	{
		if (weapon == weapons.at(i))
		{
			weapons.erase(weapons.begin() + i);
		}
	}
}

AWeapon* Inventory::GetWeapon(int number)
{

	//return nullptr;
	return weapons.at(number);
}
