// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMPlayerState.h"
#include "CMGameMode.h"
#include "CMGameMode_Hunt.h"
#include "UnrealNetwork.h"

ACMPlayerState::ACMPlayerState(const FObjectInitializer& objectInitializer)
: Super(objectInitializer)
{
	alive = false;
	huntTarget = NULL;
	bReplicates = true;

	ResetStats();
}

void ACMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACMPlayerState, alive);
	DOREPLIFETIME(ACMPlayerState, colors);
	DOREPLIFETIME(ACMPlayerState, team);
	DOREPLIFETIME(ACMPlayerState, huntTarget);
	DOREPLIFETIME(ACMPlayerState, huntTargetColor);
	DOREPLIFETIME(ACMPlayerState, frags);
	DOREPLIFETIME(ACMPlayerState, deaths);
	DOREPLIFETIME(ACMPlayerState, money);
};

void ACMPlayerState::ResetStats()
{
	frags = 0;
	deaths = 0;
	huntTarget = NULL;
	money = 0;
}

int32 ACMPlayerState::GetTeam()
{
	return team;
}

void ACMPlayerState::SetTeam(int32 newTeam)
{
	team = newTeam;
}

void ACMPlayerState::SetHuntTarget(AController* target, FLinearColor targetColor)
{
	if (targetColor == FLinearColor::Transparent && target != NULL)
	{
		ACMPlayerState* playerState = Cast<ACMPlayerState>(target->PlayerState);
		if (playerState != NULL)
			targetColor = playerState->GetColorId();
	}

	huntTarget = target;
	huntTargetColor = targetColor;
}

AController* ACMPlayerState::GetHuntTargetController()
{
	return huntTarget;
}

FLinearColor ACMPlayerState::GetHuntTargetColor()
{
	return huntTargetColor;
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

int32 ACMPlayerState::GetMoney()
{
	return money;
}

void ACMPlayerState::AddMoney(int32 num)
{
	SetMoney(money + num);
}

void ACMPlayerState::SetMoney(int32 num)
{
	int32 maxMoney = MAX_int32;
	ACMGameMode* gameMode = Cast<ACMGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode != NULL)
		maxMoney = gameMode->GetPlayerMaxMoney();

	money = num;

	if (money < 0)
		money = 0;
	
	if (money > maxMoney)
		money = maxMoney;
}

bool ACMPlayerState::IsAlive()
{
	return alive;
}

void ACMPlayerState::SetAlive(bool alive)
{
	this->alive = alive;
}

FLinearColor ACMPlayerState::GetColor(PlayerColor colorComponent)
{
	return colors[(uint8)colorComponent];
}

FLinearColor ACMPlayerState::GetColorId()
{
	return GetColor(PlayerColorIdentifier);
}

TArray<FLinearColor> ACMPlayerState::GetColors()
{
	TArray<FLinearColor> array;
	for (FLinearColor color : colors)
		array.Add(color);

	return array;
}

void ACMPlayerState::SetColor(FLinearColor color, PlayerColor colorComponent)
{
	colors[(uint8)colorComponent] = color;
}

void ACMPlayerState::SetColorId(FLinearColor color)
{
	SetColor(color, PlayerColorIdentifier);
}