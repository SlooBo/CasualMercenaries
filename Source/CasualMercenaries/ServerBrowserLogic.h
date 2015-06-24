// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UILogicBase.h"
#include "ServerBrowserLogic.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API UServerBrowserLogic : public UUILogicBase
{
	GENERATED_BODY()
	
public:
	UServerBrowserLogic();
	~UServerBrowserLogic();
	void SetUp(UUserWidget *widget,UWorld *world);
	UFUNCTION()
	void CreateSession();
private:
	UWorld *world;
	UUserWidget *widget;
	UButton *createSessionButton;
	
};
