// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Weapon.h"
#include "WeaponData.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class RANGE_TYPE : uint8
{
	CLOSE_RANGE = 1,
	CLOSE_MID_RANGE = 2,
	MID_RANGE = 3,
	MID_LONG_RANGE = 4,
	LONG_RANGE  = 5,
};

USTRUCT()
struct CASUALMERCENARIES_API FWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	uint16 clipSize;

	UPROPERTY()
	float reloadTime;

	UPROPERTY()
	uint16 damage;

	UPROPERTY()
	float fireRate;

	UPROPERTY()
	FString description;

	UPROPERTY()
	RANGE_TYPE range;

	UPROPERTY()
	uint32 buyPrice;

	UPROPERTY()
	uint32 sellPrice;

	UPROPERTY()
	FString displayName;

	UPROPERTY()
	FString iconPath;

	UPROPERTY()
	uint32 upgradePrice1;

	UPROPERTY()
	uint32 upgradePrice2;

	FWeaponData()
	{

	}
	FWeaponData(uint16 clipSize, float reloadTime, uint16 damage, float fireRate, RANGE_TYPE range, uint32 buyPrice,
		uint32 upgradePrice1,uint32 upgradePrice2, FString name,FString description,FString iconPath)
	{
		this->clipSize = clipSize;
		this->reloadTime = reloadTime;
		this->damage = damage;
		this->fireRate = fireRate;
		this->description = description;
		this->range = range;
		this->buyPrice = buyPrice;
		this->sellPrice = buyPrice / 2;
		this->displayName = name;
		this->iconPath = iconPath;
		this->upgradePrice1 = upgradePrice1;
		this->upgradePrice2 = upgradePrice2;
	}
	
};
