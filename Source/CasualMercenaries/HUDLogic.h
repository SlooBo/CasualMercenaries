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
	UTextBlock *clipSizeText;
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
