// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class CASUALMERCENARIES_API Util
{
public:
	static class APlayerCharacter* GetLocalPlayer(UWorld* world);
	static class APlayerCharacter* GetPlayerCharacter(APlayerController* player);
	static int32 GetNumPlayers(UWorld* world);
};
