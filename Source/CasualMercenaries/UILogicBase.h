// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "UILogicBase.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API UUILogicBase : public UObject
{
	GENERATED_BODY()
	public:
		UUILogicBase();
		~UUILogicBase();
		virtual void Update(){}
		template <typename type>
		bool SetValueFromWidget(type **saveValueHere, FString name)
		{
			UWidgetTree *widgetTree = baseWidget->WidgetTree;
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
protected:
	UPROPERTY()
	UUserWidget *baseWidget;
	
};
