// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Weapon.h"
#include "Inventory.generated.h"

/**
 * 
 */
USTRUCT()
struct CASUALMERCENARIES_API FInventory
{
	GENERATED_USTRUCT_BODY()
	
public:

	void ClearInventory();

	void AddWeaponToInventory(AWeapon* weapon);

	/*
	UNSAFE! Don't remove weapon if you want to change it! Use Changeweapon.
	*/
	void RemoveWeaponFromInventory(int number);

	void RemoveWeaponFromInventory(AWeapon* weapon);

	void ChangeWeaponAtSlot(uint16 slot, WEAPONID weaponid);




	//count begins from 0
	AWeapon* GetWeapon(int number);
	AWeapon* GetCurrentWeapon();

	void SetPlayer(APlayerCharacter *player);

	UPROPERTY()
	TArray<AWeapon*> weapons;

	void ChangeWeaponAtSlot(uint16 slot, AWeapon *newWeapon);
	UPROPERTY()
	class APlayerCharacter *owningPlayer;
	UPROPERTY()
	uint16 maxInventorySize;
	int32 currentWeapon;
private:

	
	
};
