// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "ShopLogic.h"
#include "Inventory.h"
#include "PlayerCharacter.h"
#include "Weapon.h"
#include "PlayerHud.h"
UShopLogic::UShopLogic(const FObjectInitializer& PCIP)
{

}
UShopLogic::~UShopLogic()
{

}
void UShopLogic::SetUp(UUserWidget *shopWidget,UWorld *world)
{
	this->world = world;
	shopMenu = shopWidget;
	UWidgetTree *widgetTree = shopWidget->WidgetTree;
	TArray<UWidget*> children;
	widgetTree->GetAllWidgets(children);
	
	APlayerController *playercontroller = world->GetFirstPlayerController();
	APlayerCharacter *playercharacter = Cast<APlayerCharacter>(playercontroller->GetPawn());
	playerInventory = playercharacter->GetInventory();
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
		else if (tempButton != nullptr && tempButton->GetName().Equals("QuitButton"))
		{
			tempButton->OnClicked.AddDynamic(this, &UShopLogic::OnClickedQuit);
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
	AWeapon *weapon = playerInventory->GetWeapon(slotIndex);
	if (weapon != nullptr)
	{
		switch (weapon->GetID())
		{
		case WEAPONID::ROCKET_LAUNCHER:
		{
			ChangeCurrentShopSlot((uint32)WEAPONID::ROCKET_LAUNCHER);
			break;
		}
		case WEAPONID::WATER_GUN:
		{
			ChangeCurrentShopSlot((uint32)WEAPONID::WATER_GUN);
			break;
		}
		case WEAPONID::GRENADE_LAUNCHER:
		{
			ChangeCurrentShopSlot((uint32)WEAPONID::GRENADE_LAUNCHER);
			break;
		}
		
		case WEAPONID::MASHINE_GUN:
		{
			ChangeCurrentShopSlot((uint32)WEAPONID::MASHINE_GUN);
			break;
		}
		case WEAPONID::MUDBUSTER_GUN:
		{
			ChangeCurrentShopSlot((uint32)WEAPONID::MUDBUSTER_GUN);
			break;
		}
	
		}
		
	}
}
void UShopLogic::OnClickedShopSlot(uint32 slotIndex)
{
	ChangeCurrentShopSlot(slotIndex);
	APlayerCharacter *player = Cast<APlayerCharacter>(world->GetFirstPlayerController()->GetPawn());
	player->GetInventory()->ChangeWeaponAtSlot(currentWeaponIndex, AWeapon::GetIDFromInt(slotIndex));
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
void UShopLogic::ChangeCurrentShopSlot(uint32 slotIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Clicked shopbutton on index: " + FString::FromInt(slotIndex));
	ChangeShopSlotColor(slotIndex, FLinearColor::Yellow);
	if (slotIndex != currentShopIndex)
		ChangeShopSlotColor(currentShopIndex, FLinearColor::White);
	currentShopIndex = slotIndex;
}
void UShopLogic::OnClickedQuit()
{
	Cast<APlayerHud>(world->GetFirstPlayerController()->GetHUD())->changeUIElement(MenuType::GAME_UI);
}