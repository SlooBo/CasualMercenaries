// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Chat.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API UChat : public UObject
{
	GENERATED_BODY()
public:
	UChat(const FObjectInitializer& PCIP);
	~UChat();
	void Initialize(UUserWidget *chatWidget, UWorld *world);
	void AddText(FString text);

	void OpenAllChat();
	void OpenTeamChat();
	void ChangePlayerInputVisibility();
	UFUNCTION(BlueprintCallable, Category = "Chat")
	bool GetIsInputVisible();
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SetIsInputVisible(bool value);
private:
	bool isInputVisible;
	UWorld *world;
	UClass *chatText;
	UUserWidget *chatWidget;
	UScrollBox *textBox;
	
	void SetInputModeGameAndUI();
	
};
