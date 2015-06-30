// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UILogicBase.h"
#include "ShopLogic.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API UShopLogic : public UUILogicBase
{
	GENERATED_BODY()
public:
	UShopLogic(const FObjectInitializer& PCIP);
	~UShopLogic();
	void SetUp(UUserWidget *shopWidget, UWorld *world);
	UFUNCTION()
	void OnClickedWeaponSlot(uint32 slotIndex);
	UFUNCTION()
	void OnClickedShopSlot(uint32 slotIndex);
private:
	UPROPERTY()
	UUserWidget *shopMenu;
	UPROPERTY()
	TArray<UButton*> weaponSlotButtons;
	UPROPERTY()
	TArray<UButton*> shopSlotButtons;
	UFUNCTION()
	UButton* getShopButton(uint32 index);
	UFUNCTION()
	UButton* getWeaponSlotButton(uint32 index);
	UPROPERTY()
	uint32 currentShopIndex;
	UPROPERTY()
	uint32 currentWeaponIndex;
	UFUNCTION()
	void ChangeShopSlotColor(uint32 index, FLinearColor color);
	UFUNCTION()
	void ChangeWeaponSlotColor(uint32 index, FLinearColor color);
	UPROPERTY()
	TArray<class UShopSlot*> shopSlots;
	UPROPERTY()
	TArray<class UWeaponSlot*> weaponSlots;
	UPROPERTY()
	class UInventory *playerInventory;
	UPROPERTY()
	UWorld *world;
	UFUNCTION()
	void ChangeCurrentShopSlot(uint32 slotIndex);
};
UCLASS()
class UWeaponSlot: public UObject
{
	GENERATED_BODY()

public:

	int32 slotIndex;
	UShopLogic *shopLogic;
	UWeaponSlot()
	{
	}
	UFUNCTION()
		void OnClicked()
	{
		shopLogic->OnClickedWeaponSlot(slotIndex);
	}
};

UCLASS()
class UShopSlot : public UObject
{
	GENERATED_BODY()

public:

	int32 slotIndex;
	UShopLogic *shopLogic;
	UShopSlot()
	{
	}
	UFUNCTION()
		void OnClicked()
	{
		shopLogic->OnClickedShopSlot(slotIndex);
	}
};