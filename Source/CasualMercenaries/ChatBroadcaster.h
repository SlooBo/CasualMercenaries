// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "ChatBroadcaster.generated.h"

/**
*
*/
UCLASS()
class CASUALMERCENARIES_API UChatBroadcaster : public UObject
{
	GENERATED_BODY()

public:
	UChatBroadcaster(const FObjectInitializer& PCIP);
	~UChatBroadcaster();
	void HandleMessage(const FString message);
	void SendAllChat(const FString message);
	void SendTeamChat(const FString message, uint32 teamId);

};
