// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "ShopLogic.h"




UShopLogic::UShopLogic()
{

}
UShopLogic::~UShopLogic()
{

}
void UShopLogic::SetUp(UUserWidget *shopWidget)
{
	shopMenu = shopWidget;
	UWidgetTree *widgetTree = shopWidget->WidgetTree;
	TArray<UWidget*> children;
	widgetTree->GetAllWidgets(children);


	int childcount = children.Num();
	for (int i = 0; i < childcount; i++)
	{
		UButton *tempButton= Cast<UButton>(children[i]);
		if (tempButton != nullptr && tempButton->GetName().StartsWith("Weapon"))
		{
			FString *left = new FString();
			FString *right = new FString();
			bool success = tempButton->GetName().Split("Weapon", left, right);
			
			UWeaponSlot* weaponSlot = NewObject<UWeaponSlot>();
			uint32 index = FCString::Atoi(**right);
			weaponSlot->slotIndex = index;
			weaponSlot->shopLogic = this;

			tempButton->OnClicked.AddDynamic(weaponSlot, &UWeaponSlot::OnClicked);
			weaponSlotButtons.Add(Cast<UButton>(children[i]));
			weaponSlots.Add(weaponSlot);
		}
		else if (tempButton != nullptr && tempButton->GetName().StartsWith("ShopItem"))
		{
			FString *left = new FString();
			FString *right = new FString();
			bool success = tempButton->GetName().Split("ShopItem", left, right);

			UShopSlot* shopSlot = NewObject<UShopSlot>();
			uint32 index = FCString::Atoi(**right);
			shopSlot->slotIndex = index;
			shopSlot->shopLogic = this;

			tempButton->OnClicked.AddDynamic(shopSlot, &UShopSlot::OnClicked);
			shopSlotButtons.Add(Cast<UButton>(children[i]));
			shopSlots.Add(shopSlot);
		}
	}
}
void UShopLogic::OnClickedWeaponSlot(uint32 slotIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Clicked weaponslot on index: " + FString::FromInt(slotIndex));
	ChangeWeaponSlotColor(slotIndex, FLinearColor::Yellow);
	if (slotIndex != currentWeaponIndex)
		ChangeWeaponSlotColor(currentWeaponIndex, FLinearColor::White);
	currentWeaponIndex = slotIndex;
}
void UShopLogic::OnClickedShopSlot(uint32 slotIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Clicked shopbutton on index: " + FString::FromInt(slotIndex));
	ChangeShopSlotColor(slotIndex, FLinearColor::Yellow);
	if (slotIndex != currentShopIndex)
	ChangeShopSlotColor(currentShopIndex, FLinearColor::White);
	currentShopIndex = slotIndex;
}
UButton* UShopLogic::getShopButton(uint32 index)
{
	for (int i = 0; i < shopSlotButtons.Num(); i++)
	{
		FString *left = new FString();
		FString *right = new FString();
		bool success = shopSlotButtons[i]->GetName().Split("ShopItem", left, right);
		uint32 shopindex = FCString::Atoi(**right);
		if (index == shopindex)
		{
			return shopSlotButtons[i];
		}
	}
	return nullptr;
}
UButton* UShopLogic::getWeaponSlotButton(uint32 index)
{
	for (int i = 0; i < weaponSlotButtons.Num(); i++)
	{
		FString *left = new FString();
		FString *right = new FString();
		bool success = weaponSlotButtons[i]->GetName().Split("Weapon", left, right);
		uint32 slotindex = FCString::Atoi(**right);
		if (index == slotindex)
		{
			return weaponSlotButtons[i];
		}
	}
	return nullptr;
}
void UShopLogic::ChangeShopSlotColor(uint32 index, FLinearColor color)
{
	UButton* tempButton = getShopButton(index);
	if (tempButton != nullptr)
	{
		tempButton->SetBackgroundColor(color);
	}
}
void UShopLogic::ChangeWeaponSlotColor(uint32 index, FLinearColor color)
{
	UButton* tempButton = getWeaponSlotButton(index);
	if (tempButton != nullptr)
	{
		tempButton->SetBackgroundColor(color);
	}
}