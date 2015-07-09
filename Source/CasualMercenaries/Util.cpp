// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Util.h"
#include "PlayerCharacter.h"
APlayerCharacter* Util::GetLocalPlayer(UWorld* world)
{
	return Cast<APlayerCharacter>(world->GetFirstPlayerController()->GetPawn());
}

APlayerCharacter* Util::GetPlayerCharacter(APlayerController* player)
{
	return Cast<APlayerCharacter>(player->GetPawn());
}

int32 Util::GetNumPlayers(UWorld* world)
{
	int32 numPlayers = 0;

	for (FConstPlayerControllerIterator iter = world->GetPlayerControllerIterator(); iter; ++iter)
		numPlayers++;

	return numPlayers;
}
