// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "HUDLogic.h"

#include "PlayerCharacter.h"
#include "CMPlayerController.h"
#include "WeaponData.h"
#include "CMPlayerState.h"
#include "Util.h"
UHUDLogic::UHUDLogic()
{

}
UHUDLogic::~UHUDLogic()
{

}

void UHUDLogic::SetUp(UUserWidget *widget, UWorld *world)
{
	thisWidget = widget;
	this->world = world;
	SetValueFromWidget(&healthProgressBar, "HealthProgressBar");
	SetValueFromWidget(&staminaProgressBar, "StaminaProgressBar");
	SetValueFromWidget(&armorProgressBar, "ArmorProgressBar");

	SetValueFromWidget(&healthText, "HealthText");
	SetValueFromWidget(&staminaText, "StaminaText");
	SetValueFromWidget(&armorText, "ArmorText");

	SetValueFromWidget(&currentAmmoText, "CurrentAmmo");
	SetValueFromWidget(&clipSizeText, "ClipSize");
	SetValueFromWidget(&cashText, "CashText");

	SetValueFromWidget(&weaponIcon, "WeaponIcon");
}

void UHUDLogic::Update()
{
	APlayerCharacter *player = Cast<APlayerCharacter>(world->GetFirstPlayerController()->GetPawn());
	if (player == nullptr)
		return;
	healthProgressBar->SetPercent(player->GetHealth() / player->GetHealthMax());
	staminaProgressBar->SetPercent(player->GetStamina() / player->GetStaminaMax());
	armorProgressBar->SetPercent(player->GetArmor() / player->GetArmorMax());

	healthText->SetText(FText::FromString(FString::FromInt((int32)player->GetHealth())));
	staminaText->SetText(FText::FromString(FString::FromInt((int32)player->GetStamina())));
	armorText->SetText(FText::FromString(FString::FromInt((int32)player->GetArmor())));

	

	ACMPlayerController *controller = Cast<ACMPlayerController>(world->GetFirstPlayerController());
	AWeapon *currentWeapon = controller->GetInventory().GetWeapon(controller->GetInventory().currentWeapon);
	if (currentWeapon != nullptr)
	{
		FWeaponStruct *weaponStruct = WeaponData::Get()->GetWeaponData(currentWeapon->GetID());
		currentAmmoText->SetText(FText::FromString(FString::FromInt(currentWeapon->GetAmmo())));
		clipSizeText->SetText(FText::FromString(FString::FromInt(weaponStruct->clipSize)));

		UTexture2D *icon = Util::LoadObjFromPath<UTexture2D>(FName(*weaponStruct->iconPath));
		weaponIcon->Brush.SetResourceObject(icon);
	}
	else
	{
		currentAmmoText->SetText(FText::FromString(FString::FromInt(0)));
		clipSizeText->SetText(FText::FromString(FString::FromInt(0)));

		FWeaponStruct *weaponStruct = WeaponData::Get()->GetWeaponData(WEAPONID::NO_WEAPON);
		UTexture2D *icon = Util::LoadObjFromPath<UTexture2D>(FName(*weaponStruct->iconPath));
		weaponIcon->Brush.SetResourceObject(icon);
	}

	ACMPlayerState *playerState = Cast<ACMPlayerState>(controller->PlayerState);
	cashText->SetText(FText::FromString(FString::FromInt(playerState->GetMoney()) + " $"));
	//currentAmmo->SetText(cont)
}