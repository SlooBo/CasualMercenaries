// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "ChatBroadcaster.h"
#include "PlayerCharacter.h"



UChatBroadcaster::UChatBroadcaster(const FObjectInitializer& PCIP)
{

}
UChatBroadcaster::~UChatBroadcaster()
{

}
void UChatBroadcaster::HandleMessage(const FString message)
{

	//First message is converted to lowercase, then it is split from space
	//This leaves out beginning command (like for example /all or /team
	//and text is whatever text/ command argument is left

	//EXAMPLE:
	// /all Hello World!
	//command would be '/all'
	//text would be 'hello world!'
	FString tempMessage = message.ToLower();

	FString spaceChar = " ";
	FString *command = new FString();
	FString *text = new FString();

	tempMessage.Split(spaceChar, command, text, ESearchCase::CaseSensitive, ESearchDir::FromStart);

	if (command->Equals("/all"))
	{
		SendAllChat(*text);
	}
	else if (command->Equals("/team"))
	{
		SendTeamChat(message, 0);
	}

}
void UChatBroadcaster::SendAllChat(const FString message)
{
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		APlayerCharacter* playerCharacter = static_cast<APlayerCharacter*>((*iter)->GetPawn());
		if (playerCharacter != NULL)
			playerCharacter->ReceiveChat(message);
	}
}

void UChatBroadcaster::SendTeamChat(const FString message, uint32 teamId)
{

}