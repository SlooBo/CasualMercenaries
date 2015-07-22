// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UILogicBase.h"
#include "HUDLogic.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API UHUDLogic : public UUILogicBase
{
	GENERATED_BODY()
	
public:
	UHUDLogic();
	~UHUDLogic();
	UFUNCTION()
	void SetUp(UUserWidget *widget, UWorld *world);
	
	void Update() override;
private:
	UWorld *world;
	UProgressBar *healthProgressBar;
	UProgressBar *staminaProgressBar;
	UProgressBar *cashProgressBar;
	UProgressBar *ammoProgressBar;
	UTextBlock *healthText;
	UTextBlock *staminaText;

	UTextBlock *currentAmmoText;
	UTextBlock *cashText;
	UTextBlock *roundTimeLeft;
	UTextBlock *currentRoundText;
	UImage *weaponIcon;
	UImage *targetSphere;

};
