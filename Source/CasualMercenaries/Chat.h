// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */

class CASUALMERCENARIES_API Chat
{
public:
	Chat(UClass *chatText,  UWorld *world);
	~Chat();
	void Initialize(UUserWidget *chatWidget);
	void AddText(FString text);

	void OpenAllChat();
	void OpenTeamChat();
private:
	UWorld *world;
	UClass *chatText;
	UUserWidget *chatWidget;
	bool isBeingUpdated;

	UScrollBox *textBox;

};
