// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "HUDLogic.h"

#include "PlayerCharacter.h"
#include "CMPlayerController.h"
#include "WeaponData.h"
#include "CMPlayerState.h"
#include "Util.h"
#include "CMGameState.h"
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

	SetValueFromWidget(&healthText, "HealthText");
	SetValueFromWidget(&staminaText, "StaminaText");

	SetValueFromWidget(&currentAmmoText, "CurrentAmmo");
	SetValueFromWidget(&cashText, "CashText");

	SetValueFromWidget(&weaponIcon, "WeaponIcon");
	SetValueFromWidget(&targetSphere, "TargetSphere");

	SetValueFromWidget(&cashProgressBar, "CashProgressBar");
	SetValueFromWidget(&ammoProgressBar, "AmmoProgressBar");

	SetValueFromWidget(&roundTimeLeft, "RoundTimer");
}

void UHUDLogic::Update()
{
	APlayerCharacter *player = Cast<APlayerCharacter>(world->GetFirstPlayerController()->GetPawn());
	if (player == nullptr)
		return;
	healthProgressBar->SetPercent(player->GetHealth() / player->GetHealthMax());
	staminaProgressBar->SetPercent(player->GetStamina() / player->GetStaminaMax());

	healthText->SetText(FText::FromString(FString::FromInt((int32)player->GetHealth())));
	staminaText->SetText(FText::FromString(FString::FromInt((int32)player->GetStamina())));
	

	ACMPlayerController *controller = Cast<ACMPlayerController>(world->GetFirstPlayerController());
	AWeapon *currentWeapon = controller->GetInventory().GetWeapon(controller->GetInventory().currentWeapon);
	if (currentWeapon != nullptr)
	{
		FWeaponStruct *weaponStruct = WeaponData::Get()->GetWeaponData(currentWeapon->GetID());
		currentAmmoText->SetText(FText::FromString(FString::FromInt(currentWeapon->GetAmmo()) + FString(" / ") + FString::FromInt(weaponStruct->clipSize)));

		UTexture2D *icon = Util::LoadObjFromPath<UTexture2D>(FName(*weaponStruct->iconPath));
		weaponIcon->Brush.SetResourceObject(icon);

		float ammoPercent = (float)currentWeapon->GetAmmo() / (float)weaponStruct->clipSize;
		ammoProgressBar->SetPercent(ammoPercent);
	}
	else
	{
		currentAmmoText->SetText(FText::FromString(FString::FromInt(0) + FString(" / ") + FString::FromInt(0)));

		FWeaponStruct *weaponStruct = WeaponData::Get()->GetWeaponData(WEAPONID::NO_WEAPON);
		UTexture2D *icon = Util::LoadObjFromPath<UTexture2D>(FName(*weaponStruct->iconPath));
		weaponIcon->Brush.SetResourceObject(icon);
		float ammoPercent = 1.0f;
		ammoProgressBar->SetPercent(ammoPercent);
	}

	ACMPlayerState *playerState = Cast<ACMPlayerState>(controller->PlayerState);
	cashText->SetText(FText::FromString(FString::FromInt(playerState->GetMoney()) + " $"));

	APawn *otherPlayerPawn = playerState->GetHuntTarget();
	if (otherPlayerPawn != nullptr)
	{
			ACMPlayerState *state = Cast<ACMPlayerState>(otherPlayerPawn->PlayerState);
			if (state != nullptr)
			{
				FLinearColor color = state->GetColorId();
				targetSphere->Brush.TintColor = color;
			}
		
	}
	float moneyPercent = playerState->GetMoney() / 5000.0f;
	cashProgressBar->SetPercent(moneyPercent);

	ACMGameState *gameState = Cast<ACMGameState>(world->GameState);
	FText text = FText::FromString(FString::FromInt(gameState->GetStateTimeleft()));
	
	roundTimeLeft->SetText(text);

	//currentAmmo->SetText(cont)
}