// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMGameState.h"
#include "UnrealNetwork.h"

ACMGameState::ACMGameState(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{

}

void ACMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACMGameState, inGameState);
	DOREPLIFETIME(ACMGameState, huntGameState);
	DOREPLIFETIME(ACMGameState, stateTimeElapsed);
	DOREPLIFETIME(ACMGameState, stateTimeLength);
	DOREPLIFETIME(ACMGameState, gameTimeElapsed);
	DOREPLIFETIME(ACMGameState, gameTimeLength);
};

int32 ACMGameState::GetStateTimeleft()
{
	return stateTimeLength - stateTimeElapsed;
}

int32 ACMGameState::GetGameTimeleft()
{
	return gameTimeLength - gameTimeElapsed;
}

int32 ACMGameState::GetStateTimeElapsed()
{
	return stateTimeElapsed;
}

int32 ACMGameState::GetGameTimeElapsed()
{
	return gameTimeElapsed;
}

int32 ACMGameState::GetStateTimeLength()
{
	return stateTimeLength;
}

int32 ACMGameState::GetGameTimeLength()
{
	return gameTimeLength;
}