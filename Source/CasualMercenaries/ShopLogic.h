// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UILogicBase.h"
#include "Inventory.h"
#include "WeaponData.h"
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

	UPROPERTY()
	UButton* buyButton;
	UPROPERTY()
	UButton* upgradeButton1;
	UPROPERTY()
	UButton* upgradeButton2;
	UPROPERTY()
	UTextBlock *buyButtonText;
	UPROPERTY()
	UImage* weaponImage;
	UPROPERTY()
	uint32 currentShopIndex;
	UPROPERTY()
	uint32 currentWeaponIndex;
	UPROPERTY()
	TMap<WEAPONID, FWeaponData> weaponData;
	UPROPERTY()
	TArray<class UShopSlot*> shopSlots;
	UPROPERTY()
	TArray<class UWeaponSlot*> weaponSlots;
	UPROPERTY()
	UWorld *world;
	UFUNCTION()
	UButton* getShopButton(uint32 index);
	UFUNCTION()
	UButton* getWeaponSlotButton(uint32 index);
	UFUNCTION()
	void ChangeShopSlotColor(uint32 index, FSlateColor color);
	UFUNCTION()
	void ChangeWeaponSlotColor(uint32 index, FSlateColor color);
	UFUNCTION()
	void ChangeCurrentShopSlot(uint32 slotIndex);
	UFUNCTION()
	void OnClickedQuit();
	UFUNCTION()
	void OnClickedBuyButton();
	UFUNCTION()
	void UpdateBuyButtonText();
	UFUNCTION()
	void OnClickedUpgradeButton1();
	UFUNCTION()
	void OnClickedUpgradeButton2();
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