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
	FWeaponData()
	{

	}
	FWeaponData(uint16 clipSize, float reloadTime, uint16 damage, float fireRate, RANGE_TYPE range, uint32 buyPrice, FString name,FString description)
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
		uint8 test = static_cast<uint8>(RANGE_TYPE::CLOSE_RANGE) | static_cast<uint8>(RANGE_TYPE::MID_RANGE);
		if (test & static_cast<uint8>(RANGE_TYPE::CLOSE_RANGE))
		{
			bool test = false;
		}
	}
	
};
