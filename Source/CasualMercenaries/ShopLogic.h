// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UILogicBase.h"
#include "Inventory.h"
#include "WeaponStruct.h"
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
	void Update() override;
private:
	UPROPERTY()
	TArray<UButton*> weaponSlotButtons;
	UPROPERTY()
	TArray<UButton*> shopSlotButtons;

	TArray<UImage*> weaponSlotIcons;
	UButton* buyButton;
	UButton* upgradeButton1;
	UButton* upgradeButton2;
	UTextBlock *buyButtonText;
	UImage* weaponImage;
	UTextBlock* tooltipText;
	UMultiLineEditableTextBox* descriptionTextBox;
	UMultiLineEditableTextBox* statTextBox;
	UPROPERTY()
	uint32 currentShopIndex;
	UPROPERTY()
	uint32 currentWeaponIndex;
	UPROPERTY()
	TMap<WEAPONID, FWeaponStruct> weaponData;
	UPROPERTY()
	TArray<class UShopSlot*> shopSlots;
	UPROPERTY()
	TArray<class UWeaponSlot*> weaponSlots;
	UWorld *world;
	UFUNCTION()
	UButton* getShopButton(uint32 index);
	UButton* getWeaponSlotButton(uint32 index);
	void ChangeShopSlotColor(uint32 index, FSlateColor color);
	void ChangeWeaponSlotColor(uint32 index, FSlateColor color);
	UFUNCTION()
	void ChangeCurrentShopSlot(uint32 slotIndex);
	UFUNCTION()
	void OnClickedQuit();
	UFUNCTION()
	void OnClickedBuyButton();
	UFUNCTION()
	void UpdateBuyButton();
	UFUNCTION()
	void OnClickedUpgradeButton1();
	UFUNCTION()
	void OnClickedUpgradeButton2();
	void ChangeWeaponIconImage(UTexture2D *newImage);
	void UpdateInfoBox();
	void SetInputMode_UIOnly();
	void SetInputMode_GameOnly();

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