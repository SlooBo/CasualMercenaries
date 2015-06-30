// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Weapon.h"
#include "Inventory.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API UInventory : public UObject
{
	GENERATED_BODY()
	
public:

	UInventory();
	~UInventory();

	void ClearInventory();

	void AddWeaponToInventory(AWeapon* weapon);

	/*
	UNSAFE! Don't remove weapon if you want to change it! Use Changeweapon.
	*/
	void RemoveWeaponFromInventory(int number);

	void RemoveWeaponFromInventory(AWeapon* weapon);

	void ChangeWeaponAtSlot(uint16 slot,AWeapon *newWeapon);


	//count begins from 0
	AWeapon* GetWeapon(int number);



private:
	uint16 maxInventorySize;
	UPROPERTY()
	TArray<AWeapon*> weapons;
	
	
};
