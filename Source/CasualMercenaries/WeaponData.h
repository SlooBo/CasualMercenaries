// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "WeaponStruct.h"
#include "Weapon.h"
/**
 * 
 */
class CASUALMERCENARIES_API WeaponData
{
public:
	static WeaponData *Get()
	{
		if (instance == nullptr)
			instance = new WeaponData();
		return instance;
	}
	void Initialize(); 
	FWeaponStruct *GetWeaponData(WEAPONID weaponId);
	FString GetRangeEnumString(ERANGE_TYPE value);

private:

	static WeaponData *instance;
	WeaponData();
	~WeaponData();
	TMap<WEAPONID, FWeaponStruct> weaponData;
};
