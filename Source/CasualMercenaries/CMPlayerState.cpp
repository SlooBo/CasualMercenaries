// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMPlayerState.h"

#include "Net/UnrealNetwork.h"

ACMPlayerState::ACMPlayerState(const FObjectInitializer& objectInitializer)
: Super(objectInitializer)
{
	huntTarget = NULL;
	bReplicates = true;
}

void ACMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACMPlayerState, team);
	DOREPLIFETIME(ACMPlayerState, huntTarget);
	DOREPLIFETIME(ACMPlayerState, frags);
	DOREPLIFETIME(ACMPlayerState, deaths);
};

int32 ACMPlayerState::GetTeam()
{
	return team;
}

void ACMPlayerState::SetTeam(int32 newTeam)
{
	team = newTeam;
}

void ACMPlayerState::SetHuntTarget(APawn* target)
{
	huntTarget = target;
}

APawn* ACMPlayerState::GetHuntTarget()
{
	return huntTarget;
}

int32 ACMPlayerState::GetFrags()
{
	return frags;
}

void ACMPlayerState::AddFrags(int32 num)
{
	frags += num;
}

int32 ACMPlayerState::GetDeaths()
{
	return deaths;
}

void ACMPlayerState::AddDeaths(int32 num)
{
	deaths += num;
}