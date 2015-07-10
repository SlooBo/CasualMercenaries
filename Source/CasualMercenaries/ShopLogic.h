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
	UTextBlock *cashText;
	UPROPERTY()
	UImage* weaponImage;
	UPROPERTY()
	UTextBlock* tooltipText;
	UPROPERTY()
	UMultiLineEditableTextBox* descriptionTextBox;
	UPROPERTY()
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
	template <typename type>
	bool SetValueFromWidget(type **saveValueHere, FString name)
	{
		UWidgetTree *widgetTree = shopMenu->WidgetTree;
		TArray<UWidget*> children;
		widgetTree->GetAllWidgets(children);

		int childcount = children.Num();
		for (int i = 0; i < childcount; i++)
		{
			type *tempValue = Cast<type>(children[i]);
			UWidget *userWidget = Cast<UWidget>(children[i]);

			FString name2 = name.Left(1);
			name2 = name2.ToLower();
			name2 += name.RightChop(1);

			if (tempValue != nullptr && userWidget != nullptr &&
				(userWidget->GetName().Equals(name) || userWidget->GetName().Equals(name2)))
			{
				*saveValueHere = Cast<type>(children[i]);
				return true;
			}
		}
		return false;
	}
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