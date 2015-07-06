// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "ShopLogic.h"
#include "Inventory.h"
#include "PlayerCharacter.h"
#include "Weapon.h"
#include "PlayerHud.h"
#include "Util.h"
UShopLogic::UShopLogic(const FObjectInitializer& PCIP)
{
	//	FWeaponData(uint16 clipSize, float reloadTime, uint16 damage, float fireRate, RANGE_TYPE range, uint32 buyPrice,
	//uint32 upgradePrice1, uint32 upgradePrice2, FString name, FString description, FString iconPath)
	FWeaponData rocketLauncher = FWeaponData(5, 10, 15, 20, RANGE_TYPE::CLOSE_RANGE, 100,200,300, "Rocket Launcher", "shoots rockets","Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::ROCKET_LAUNCHER, rocketLauncher);

	FWeaponData waterGun = FWeaponData(5, 10, 15, 20, RANGE_TYPE::CLOSE_RANGE, 100,200,300, "Water Gun", "shoots water","Texture2D'/Game/Game/UI/Textures/watergunPic.watergunPic'");
	weaponData.Add(WEAPONID::WATER_GUN, waterGun);

	FWeaponData grenadeLauncher = FWeaponData(5, 10, 15, 20, RANGE_TYPE::CLOSE_RANGE, 100,200,300, "Grenade Launcher", "Shoots shit","Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::GRENADE_LAUNCHER, grenadeLauncher);

	FWeaponData mashineGun = FWeaponData(5, 10, 15, 20, RANGE_TYPE::CLOSE_RANGE, 100,200,300, "Mashine Gun", "shoots mashines","Texture2D'/Game/Game/UI/Textures/GunPic.GunPic'");
	weaponData.Add(WEAPONID::MASHINE_GUN, mashineGun);

	FWeaponData mudbusterGun = FWeaponData(5, 10, 15, 20, RANGE_TYPE::CLOSE_RANGE, 100,200,300, "Mudbuster Gun", "shoots mudbusters","Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::MUDBUSTER_GUN, mudbusterGun);

	FWeaponData waspGun = FWeaponData(5, 10, 15, 20, RANGE_TYPE::CLOSE_RANGE, 100,200,300, "Wasp Gun", "shoots wasps","Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::WASP_GUN, waspGun);

	FWeaponData twisterGun = FWeaponData(5, 10, 15, 20, RANGE_TYPE::CLOSE_RANGE, 100,200,300, "Twister Torpedo", "shoots torpedo","Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::TWISTER_GUN, twisterGun);

	FWeaponData shotGun = FWeaponData(5, 10, 15, 20, RANGE_TYPE::CLOSE_RANGE, 100, 200, 300, "Shot Gun", "Shoots shots", "Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::SHOT_GUN, shotGun);

	FWeaponData noWeapon = FWeaponData(-1, -1, -1, -1, RANGE_TYPE::CLOSE_RANGE, -1,-1,-1, "", "","Texture2D'/Game/Game/UI/Textures/No_Weapon.No_Weapon'");
	weaponData.Add(WEAPONID::NO_WEAPON, noWeapon);

}
UShopLogic::~UShopLogic()
{

}
void UShopLogic::SetUp(UUserWidget *shopWidget,UWorld *world)
{
	this->world = world;
	shopMenu = shopWidget;

	SetValueFromWidget<UButton>(&upgradeButton1, "UpgradeButton1");
	upgradeButton1->OnClicked.AddDynamic(this, &UShopLogic::OnClickedUpgradeButton1);

	SetValueFromWidget<UButton>(&upgradeButton2, "UpgradeButton2");
	upgradeButton2->OnClicked.AddDynamic(this, &UShopLogic::OnClickedUpgradeButton2);

	UButton* quitButton = nullptr;
	SetValueFromWidget<UButton>(&quitButton, "QuitButton");
	quitButton->OnClicked.AddDynamic(this, &UShopLogic::OnClickedQuit);

	SetValueFromWidget<UButton>(&buyButton, "BuyButton");
	buyButton->OnClicked.AddDynamic(this, &UShopLogic::OnClickedBuyButton);

	SetValueFromWidget<UTextBlock>(&buyButtonText, "BuyButtonText");

	SetValueFromWidget<UImage>(&weaponImage, "SelectedWeaponImage");

	UButton *tempWeaponButton = nullptr;
	for (int i = 0; i < 4; i++)
	{
		SetValueFromWidget<UButton>(&tempWeaponButton, "Weapon" + FString::FromInt(i));
		if (tempWeaponButton == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Error during shop creation!");
			return;
		}
		UWeaponSlot* weaponSlot = NewObject<UWeaponSlot>();
		weaponSlot->slotIndex = i;
		weaponSlot->shopLogic = this;
		tempWeaponButton->OnClicked.AddDynamic(weaponSlot, &UWeaponSlot::OnClicked);

		weaponSlotButtons.Add(tempWeaponButton);
		weaponSlots.Add(weaponSlot);
	}
	UButton *tempShopButton = nullptr;
	for (int i = 0; i < (uint32)WEAPONID::NO_WEAPON - 1; i++)
	{
		SetValueFromWidget<UButton>(&tempShopButton, "ShopItem" + FString::FromInt(i));
		if (tempShopButton == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Not enough shop buttons!");
			continue;
		}
		UShopSlot* shopSlot = NewObject<UShopSlot>();
		shopSlot->slotIndex = i;
		shopSlot->shopLogic = this;

		tempShopButton->OnClicked.AddDynamic(shopSlot, &UShopSlot::OnClicked);
		shopSlotButtons.Add(tempShopButton);
		shopSlots.Add(shopSlot);
	}

	OnClickedWeaponSlot(0);
}
void UShopLogic::OnClickedWeaponSlot(uint32 slotIndex)
{
	ChangeWeaponSlotColor(slotIndex, FLinearColor::Yellow);
	if (slotIndex != currentWeaponIndex)
		ChangeWeaponSlotColor(currentWeaponIndex, FLinearColor::White);
	currentWeaponIndex = slotIndex;
	AWeapon *weapon = Cast<APlayerCharacter>(world->GetFirstPlayerController()->GetPawn())->GetInventory().GetWeapon(slotIndex);
	if (weapon != nullptr)
	{
		ChangeCurrentShopSlot((uint32)weapon->GetID());
	}
	//player has no weapon on this weapon slot
	else
	{
		upgradeButton1->SetIsEnabled(false);
		upgradeButton2->SetIsEnabled(false);
		buyButton->SetIsEnabled(false);
		FLinearColor color = FLinearColor(0.8f, 0.5f, 0.0f, 0.0f);
		FSlateColor slateColor = FSlateColor(color);
		ChangeShopSlotColor(currentShopIndex, slateColor);
		currentShopIndex = (uint32)WEAPONID::NO_WEAPON;
		UpdateInfoBox();
	}
}
void UShopLogic::OnClickedShopSlot(uint32 slotIndex)
{
	ChangeCurrentShopSlot(slotIndex);
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
void UShopLogic::ChangeShopSlotColor(uint32 index, FSlateColor color)
{
	UButton* tempButton = getShopButton(index);
	if (tempButton != nullptr)
	{
		tempButton->WidgetStyle.Normal.TintColor = color;
		tempButton->WidgetStyle.Hovered.TintColor == color;
	}
}
void UShopLogic::ChangeWeaponSlotColor(uint32 index, FSlateColor color)
{
	UButton* tempButton = getWeaponSlotButton(index);
	if (tempButton != nullptr)
	{
		tempButton->WidgetStyle.Normal.TintColor = color;
	}
}
void UShopLogic::ChangeCurrentShopSlot(uint32 slotIndex)
{
	buyButton->SetIsEnabled(true);
	upgradeButton1->SetIsEnabled(true);
	upgradeButton2->SetIsEnabled(true);
	FLinearColor color = FLinearColor::Yellow;
	color = FLinearColor(0.8f, 0.5f, 0.0f, 1.0f);
	FSlateColor slateColor = FSlateColor(color);
	ChangeShopSlotColor(slotIndex, slateColor);
	if (slotIndex != currentShopIndex && currentShopIndex != -1 && currentShopIndex != (uint32)WEAPONID::NO_WEAPON)
	{
		color = FLinearColor(1.0f, 1.0f, 1.0f, 0.0f);
		slateColor = FSlateColor(color);
		ChangeShopSlotColor(currentShopIndex, slateColor);
	}
	currentShopIndex = slotIndex;
	UpdateBuyButtonText();
	UpdateInfoBox();
}
void UShopLogic::OnClickedQuit()
{
	Cast<APlayerHud>(world->GetFirstPlayerController()->GetHUD())->changeUIElement(MenuType::GAME_UI);
}
void UShopLogic::OnClickedBuyButton()
{
	ACMPlayerController *player = Cast<ACMPlayerController>(world->GetFirstPlayerController());
	player->BuyWeapon(currentWeaponIndex,AWeapon::GetIDFromInt(currentShopIndex));
	UpdateBuyButtonText();
}
void UShopLogic::UpdateBuyButtonText()
{
	AWeapon *weapon = Cast<APlayerCharacter>(world->GetFirstPlayerController()->GetPawn())->GetInventory().GetWeapon(currentWeaponIndex);
	if (weapon != nullptr)
	{
		buyButtonText->SetText(FText::FromString("1000$"));
	}

}
void UShopLogic::OnClickedUpgradeButton1()
{

	//todo upgrade
}
void UShopLogic::OnClickedUpgradeButton2()
{
	//todo upgrade
}
void UShopLogic::ChangeWeaponIconImage(UTexture2D *newImage)
{
	weaponImage->Brush.SetResourceObject(newImage);
}
FWeaponData *UShopLogic::GetWeaponData(WEAPONID weaponId)
{
	FWeaponData *result = nullptr;
	result = weaponData.Find(weaponId);
	return result;
}
void UShopLogic::UpdateInfoBox()
{
	WEAPONID weaponid = AWeapon::GetIDFromInt((int8)currentShopIndex);
	FWeaponData *currentWeaponData = GetWeaponData(weaponid);
	if (currentWeaponData != nullptr)
	{
		UTexture2D *test = Util::LoadObjFromPath<UTexture2D>(FName(*currentWeaponData->iconPath));
		ChangeWeaponIconImage(test);
	}
}