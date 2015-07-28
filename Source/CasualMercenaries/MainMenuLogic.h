// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UILogicBase.h"
#include "MainMenuLogic.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API UMainMenuLogic : public UUILogicBase
{
	GENERATED_BODY()
	
public:
	UMainMenuLogic();
	~UMainMenuLogic();
	
	UFUNCTION()
	void SetUp(UUserWidget *widget, UWorld *world);
	
	FString GetProjectVersion();
private:
	UWorld *world;
	
	UTextBlock *versionText;
};
