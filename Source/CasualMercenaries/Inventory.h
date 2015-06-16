// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Weapon.h"
#include <vector>

/**
 * 
 */
class CASUALMERCENARIES_API Inventory
{
public:

	Inventory();
	~Inventory();

	void AddWeaponToInventory(AWeapon* weapon);

	//count begins from 0
	void RemoveWeaponFromInventpry(int number);

	void RemoveWeaponFromInventory(AWeapon* weapon);

	//count begins from 0
	AWeapon* GetWeapon(int number);

private:

	std::vector<AWeapon*> weapons;
};
