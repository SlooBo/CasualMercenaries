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
	static void Initialize(); 
	static FWeaponStruct *GetWeaponData(WEAPONID weaponId);
	static FString GetRangeEnumString(ERANGE_TYPE value);
private:
	static bool initialized;
	static TMap<WEAPONID, FWeaponStruct> weaponData;
};
