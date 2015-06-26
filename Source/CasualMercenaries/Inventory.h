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

	//count begins from 0
	void RemoveWeaponFromInventory(int number);

	void RemoveWeaponFromInventory(AWeapon* weapon);




	//count begins from 0
	AWeapon* GetWeapon(int number);



private:


	UPROPERTY()
	TArray<AWeapon*> weapons;
	
	
};
