// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "HUDLogic.h"

#include "PlayerCharacter.h"
#include "CMPlayerController.h"

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

	SetValueFromWidget(&currentAmmo, "CurrentAmmo");
	SetValueFromWidget(&clipSize, "ClipSize");
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
	//uint32 currentAmmo = controller->GetInventory().GetWeapon(player->current)
	//currentAmmo->SetText(cont)
}