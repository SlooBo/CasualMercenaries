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
	UUserWidget *thisWidget;
	UWorld *world;
	UProgressBar *healthProgressBar;
	UProgressBar *staminaProgressBar;
	UProgressBar *armorProgressBar;
	UTextBlock *healthText;
	UTextBlock *staminaText;
	UTextBlock *armorText;

	UTextBlock *currentAmmoText;
	UTextBlock *cashText;
	UImage *weaponIcon;
	UImage *targetSphere;
	template <typename type>
	bool SetValueFromWidget(type **saveValueHere, FString name)
	{
		UWidgetTree *widgetTree = thisWidget->WidgetTree;
		TArray<UWidget*> children;
		widgetTree->GetAllWidgets(children);

		int childcount = children.Num();
		for (int i = 0; i < childcount; i++)
		{
			type *tempValue = Cast<type>(children[i]);
			UWidget *userWidget = Cast<UWidget>(children[i]);
			if (tempValue != nullptr && userWidget != nullptr && userWidget->GetName().ToLower().Equals(name.ToLower()))
			{
				*saveValueHere = Cast<type>(children[i]);
				return true;
			}
		}
		return false;
	}
};
