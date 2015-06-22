// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define STRINGIFY(str) L#str

/**
 * 
 */
class CASUALMERCENARIES_API Util
{
public:
	static class APlayerCharacter* GetLocalPlayer(UWorld* world);
	static class APlayerCharacter* GetPlayerCharacter(APlayerController* player);
	static int32 GetNumPlayers(UWorld* world);
	static class AChatData* GetChatData(UWorld* world);
};