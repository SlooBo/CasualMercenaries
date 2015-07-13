// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "WeaponData.h"

WeaponData *WeaponData::instance;
void WeaponData::Initialize()
{
	//	FWeaponData(uint16 clipSize, float reloadTime, uint16 damage, float fireRate, RANGE_TYPE range, uint32 buyPrice,
	//uint32 upgradePrice1, uint32 upgradePrice2, FString name, FString description, FString iconPath)
	FWeaponStruct rocketLauncher = FWeaponStruct(
		4, 4, 50, 0.75,
		ERANGE_TYPE::MID_LONG_RANGE,
		1000, 1500, 500,
		"Rocket Launcher 'Babys first RPG'",
		"Your generic rocket launcher stolen right from quake",
		"Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'"
		);
	weaponData.Add(WEAPONID::ROCKET_LAUNCHER, rocketLauncher);

	FWeaponStruct waterGun = FWeaponStruct(
		100, .05, 0, .25,
		ERANGE_TYPE::CLOSE_RANGE,
		500, 300, 1000,
		"Wasserwerfer 'Vermuser'",
		"Es werfst wasser 'Soil everything on the field, towel not included'", //sunscreen also not included! 
		"Texture2D'/Game/Game/UI/Textures/watergunPic.watergunPic'");
	weaponData.Add(WEAPONID::WATER_GUN, waterGun);

	FWeaponStruct grenadeLauncher = FWeaponStruct(
		12, 3, 25, .5,
		ERANGE_TYPE::MID_RANGE,
		800, 800, 600,
		"Grenade Launcher 'Pomegranade launcher'",
		"Contrary to popular beliefs it does not shoot pomegranates. It shoots fire hydrants",
		"Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::GRENADE_LAUNCHER, grenadeLauncher);

	FWeaponStruct mashineGun = FWeaponStruct(
		30, 2, 10, .10,
		ERANGE_TYPE::CLOSE_MID_RANGE,
		200, 200, 300,
		"Mashine Gun 'Budda bud'",
		"Shoots mashines which is zulu for machine",
		"Texture2D'/Game/Game/UI/Textures/GunPic.GunPic'");
	weaponData.Add(WEAPONID::MASHINE_GUN, mashineGun);

	FWeaponStruct mudbusterGun = FWeaponStruct(
		5, 1, 0, .75,
		ERANGE_TYPE::MID_RANGE,
		100, 200, 300,
		"Mudbuster Gun 'Liero's dröm'",
		"Insta mud instantly just point and shoot! Where ever you need, what ever you want! Aproved by Miika the Great",
		"Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::MUDBUSTER_GUN, mudbusterGun);

	FWeaponStruct waspGun = FWeaponStruct(
		100, 10, 15, .5,
		ERANGE_TYPE::CLOSE_RANGE,
		200, 1000, 600,
		"Wasp Stick 'WaspNestCudgel'",
		"Makes you the Queen/King bee, sent forth thine minions!",
		"Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::WASP_GUN, waspGun);

	FWeaponStruct twisterGun = FWeaponStruct(
		4, 5, 30, 1.5,
		ERANGE_TYPE::CLOSE_MID_RANGE,
		700, 200, 9000,
		"Twister Torpedo 'Twisted Twister Sisters'",
		"Fling your enemies and their weapons with power of the wind",
		"Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::TWISTER_GUN, twisterGun);

	FWeaponStruct shotGun = FWeaponStruct(
		6, 1, 15, 0.5,
		ERANGE_TYPE::CLOSE_RANGE,
		300, 2300, 300,
		"Shot Gun  'Hyper Shotgun'",
		"Shoots shots of hyperventilated lead",
		"Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::SHOT_GUN, shotGun);

	FWeaponStruct noWeapon = FWeaponStruct(0, 0, 0, 0, ERANGE_TYPE::CLOSE_RANGE, 0, 0, 0, "fud", "fudud", "Texture2D'/Game/Game/UI/Textures/No_Weapon.No_Weapon'");
	weaponData.Add(WEAPONID::NO_WEAPON, noWeapon);
}
FWeaponStruct *WeaponData::GetWeaponData(WEAPONID weaponId)
{
	FWeaponStruct *result = nullptr;
	result = weaponData.Find(weaponId);
	return result;
}
FString WeaponData::GetRangeEnumString(ERANGE_TYPE value)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ERANGE_TYPE"), true);
	if (!EnumPtr) return FString("Invalid");

	return EnumPtr->GetEnumName((int32)value);
}
WeaponData::WeaponData()
{
	Initialize();
}
WeaponData::~WeaponData()
{

}