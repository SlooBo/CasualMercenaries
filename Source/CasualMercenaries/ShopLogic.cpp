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
	FWeaponData rocketLauncher = FWeaponData(
		4, 4, 50, 0.75, 
		ERANGE_TYPE::MID_LONG_RANGE,
		1000, 1500, 500, 
		"Rocket Launcher 'Babys first RPG'",
		"Your generic rocket launcher stolen right from quake",
		"Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'"
		);
	weaponData.Add(WEAPONID::ROCKET_LAUNCHER, rocketLauncher);

	FWeaponData waterGun = FWeaponData(
		100, .05, 0, .25, 
		ERANGE_TYPE::CLOSE_RANGE, 
		500, 300, 1000,
		"Wasserwerfer 'Vermuser'",
		"Es werfst wasser 'Soil everything on the field, towel not included'", //sunscreen also not included! 
		"Texture2D'/Game/Game/UI/Textures/watergunPic.watergunPic'");
	weaponData.Add(WEAPONID::WATER_GUN, waterGun);

	FWeaponData grenadeLauncher = FWeaponData(
		12, 3, 25, .5, 
		ERANGE_TYPE::MID_RANGE,
		800, 800, 600, 
		"Grenade Launcher 'Pomegranade launcher'",
		"Contrary to popular beliefs it does not shoot pomegranates. It shoots fire hydrants",
		"Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::GRENADE_LAUNCHER, grenadeLauncher);

	FWeaponData mashineGun = FWeaponData(
		30, 3, 10, .25, 
		ERANGE_TYPE::CLOSE_MID_RANGE,
		200, 200, 300, 
		"Mashine Gun 'Budda bud'",
		"Shoots mashines which is zulu for machine",
		"Texture2D'/Game/Game/UI/Textures/GunPic.GunPic'");
	weaponData.Add(WEAPONID::MASHINE_GUN, mashineGun);

	FWeaponData mudbusterGun = FWeaponData(
		5, 1, 0, .75, 
		ERANGE_TYPE::MID_RANGE,
		100, 200, 300,
		"Mudbuster Gun 'Liero's dröm'",
		"Insta mud instantly just point and shoot! Where ever you need, what ever you want! Aproved by Miika the Great",
		"Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::MUDBUSTER_GUN, mudbusterGun);

	FWeaponData waspGun = FWeaponData(
		100, 10, 15, .5,
		ERANGE_TYPE::CLOSE_RANGE,
		200, 1000, 600, 
		"Wasp Stick 'WaspNestCudgel'",
		"Makes you the Queen/King bee, sent forth thine minions!",
		"Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::WASP_GUN, waspGun);

	FWeaponData twisterGun = FWeaponData(
		4, 5, 30, 1.5, 
		ERANGE_TYPE::CLOSE_MID_RANGE,
		700, 200, 9000,
		"Twister Torpedo 'Twisted Twister Sisters'", 
		"Fling your enemies and their weapons with power of the wind",
		"Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::TWISTER_GUN, twisterGun);

	FWeaponData shotGun = FWeaponData(
		5, 10, 15, 20, 
		ERANGE_TYPE::CLOSE_RANGE,
		300, 2300, 300, 
		"Shot Gun  'Hyper Shotgun'", 
		"Shoots shots of hyperventilated lead", 
		"Texture2D'/Game/Game/UI/Textures/ShotgunPic.ShotgunPic'");
	weaponData.Add(WEAPONID::SHOT_GUN, shotGun);

	FWeaponData noWeapon = FWeaponData(0, 0, 0, 0, ERANGE_TYPE::CLOSE_RANGE, 0,0,0, "fud", "fudud","Texture2D'/Game/Game/UI/Textures/No_Weapon.No_Weapon'");
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
	SetValueFromWidget<UMultiLineEditableTextBox>(&descriptionTextBox, "DescriptionTextBox");
	SetValueFromWidget<UMultiLineEditableTextBox>(&statTextBox, "StatText");

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
	for (int i = 0; i < (uint32)WEAPONID::NO_WEAPON; i++)
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
	WEAPONID weaponid = AWeapon::GetIDFromInt((int8)currentShopIndex);
	FWeaponData *currentWeaponData = GetWeaponData(weaponid);
	if (currentWeaponData!= nullptr)
	{
		buyButtonText->SetText(FText::FromString(FString::FromInt(currentWeaponData->buyPrice) + "$"));
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
		descriptionTextBox->SetText(FText::FromString(currentWeaponData->description));
		//	FWeaponData(uint16 clipSize, float reloadTime, uint16 damage, float fireRate, RANGE_TYPE range, uint32 buyPrice,
		//uint32 upgradePrice1, uint32 upgradePrice2, FString name, FString description, FString iconPath)
		
		FString statText =
			"ClipSize: " + FString::FromInt(currentWeaponData->clipSize) +
			"\nReload Time: " + FString::FromInt(currentWeaponData->reloadTime) +
			"\nDamage: " + FString::FromInt(currentWeaponData->damage) +
			"\nFire rate: " + FString::FromInt(currentWeaponData->fireRate) +
			"\nRange: " + GetRangeEnumString(currentWeaponData->range);
		statTextBox->SetText(FText::FromString(statText));
	}
}
FString UShopLogic::GetRangeEnumString(ERANGE_TYPE value)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ERANGE_TYPE"), true);
	if (!EnumPtr) return FString("Invalid");

	return EnumPtr->GetEnumName((int32)value);
}