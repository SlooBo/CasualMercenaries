// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMGameMode_Hunt.h"
#include "CMPlayerState.h"
#include "Util.h"

ACMGameMode_Hunt::ACMGameMode_Hunt(const class FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	minPlayersToStart = 1;

	huntRounds = 2;
	huntRoundTime = 5;
	huntRoundFreezeTime = 2;
	huntIntermissionTime = 3;
}

FString ACMGameMode_Hunt::GetHuntStateAsString(HuntState state)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, STRINGIFY(HuntState), true);
	if (enumPtr == NULL)
		return FString("Invalid");
	return enumPtr->GetEnumName((uint8)state);
}

void ACMGameMode_Hunt::StartMatch()
{
	Super::StartMatch();

	if (inGameState == InGameState::Warmup)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("GameMode: Hunt"));

	TArray<APlayerController*> players;
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		if (Util::GetNumPlayers(GetWorld()) >= minPlayersToStart)
			SetRandomPlayerHuntTarget(*iter);
	}

	huntElapsed = -1;
	GetWorld()->GetTimerManager().SetTimer(huntTimerHandle, this, &ACMGameMode_Hunt::HuntTickSecond, 1.0f, true, 0.0f);
}

void ACMGameMode_Hunt::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
}

void ACMGameMode_Hunt::HuntTickSecond()
{
	huntElapsed++;
	if (GetHuntTimeLeft() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("End"));
		huntState = HuntState::End;
		huntElapsed = 0;
	}

	int32 rounds = huntElapsed / (huntRoundFreezeTime + huntRoundTime + huntIntermissionTime);
	int32 roundFreezeStart = rounds * (huntRoundFreezeTime + huntRoundTime + huntIntermissionTime);
	int32 roundStartTime = roundFreezeStart + huntRoundFreezeTime;
	int32 roundEndTime = roundStartTime + huntRoundTime;
	int32 intermissionStartTime = roundEndTime;
	int32 intermissionEndTime = intermissionStartTime + huntIntermissionTime;
	
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("State: ") + GetHuntStateAsString(huntState));

	if (huntElapsed < roundStartTime && huntRoundFreezeTime > 0 && huntState != HuntState::RoundStarting)
	{
		// freeze time started
		huntState = HuntState::RoundStarting;
		RoundFreezeStart();
		RoundStart();
	}
	else if (huntElapsed >= roundStartTime && huntElapsed < roundEndTime && huntState != HuntState::Round)
	{
		// round started
		huntState = HuntState::Round;

		if (huntRoundFreezeTime > 0)
			RoundFreezeEnd();
		else
			RoundStart();
	}
	else if (huntElapsed >= intermissionStartTime && huntElapsed < intermissionEndTime && huntState != HuntState::Intermission)
	{
		// intermission started
		huntState = HuntState::Intermission;	
		IntermissionStart();
	}
}

int32 ACMGameMode_Hunt::GetHuntTimeElapsed()
{
	return huntElapsed;
}

int32 ACMGameMode_Hunt::GetHuntRoundTimeElapsed()
{
	int32 rounds = huntElapsed / (huntRoundTime + huntIntermissionTime);
	int32 roundStartTime = rounds * (huntRoundTime + huntIntermissionTime);
	int32 roundEndTime = roundStartTime + huntRoundTime;
	int32 roundElapsed = huntElapsed - roundStartTime;

	if (huntElapsed < roundStartTime)
		return -1;

	return huntElapsed - roundStartTime;
}

int32 ACMGameMode_Hunt::GetHuntIntermissionTimeElapsed()
{
	int32 rounds = huntElapsed / (huntRoundTime + huntIntermissionTime);
	int32 intermissionEndTime = (rounds + 1) * (huntRoundTime + huntIntermissionTime);
	int32 intermissionStartTime = intermissionEndTime - huntIntermissionTime;
	int32 intermissionElapsed = huntElapsed - intermissionStartTime;

	if (huntElapsed < intermissionStartTime)
		return -1;

	return huntElapsed - intermissionStartTime;
}

int32 ACMGameMode_Hunt::GetHuntTimeLeft()
{
	int32 huntLength = (huntRoundTime * huntRounds) + (huntIntermissionTime * (huntRounds - 1));
	return huntLength - huntElapsed;
}

int32 ACMGameMode_Hunt::GetHuntRoundTimeLeft()
{
	int32 rounds = huntElapsed / (huntRoundTime + huntIntermissionTime);
	int32 roundStartTime = rounds * (huntRoundTime + huntIntermissionTime);
	int32 roundEndTime = roundStartTime + huntRoundTime;
	int32 roundElapsed = huntElapsed - roundStartTime;

	if (huntElapsed < roundStartTime)
		return -1;

	return roundEndTime - huntElapsed;
}

int32 ACMGameMode_Hunt::GetHuntIntermissionTimeLeft()
{
	int32 rounds = huntElapsed / (huntRoundTime + huntIntermissionTime);
	int32 intermissionEndTime = (rounds + 1) * (huntRoundTime + huntIntermissionTime);
	int32 intermissionStartTime = intermissionEndTime - huntIntermissionTime;
	int32 intermissionElapsed = huntElapsed - intermissionStartTime;

	if (huntElapsed < intermissionStartTime)
		return -1;

	return intermissionEndTime - huntElapsed;
}

void ACMGameMode_Hunt::RoundFreezeStart()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("Freeze time!"));
}

void ACMGameMode_Hunt::RoundFreezeEnd()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("Freeze time over"));
}

void ACMGameMode_Hunt::RoundStart()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("Round start"));
}

void ACMGameMode_Hunt::IntermissionStart()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("Intermission start"));
}

void ACMGameMode_Hunt::OnPlayerDeath_Implementation(APlayerController* player, APlayerController* killer)
{
	Super::OnPlayerDeath_Implementation(player, killer);

	if (Util::GetNumPlayers(GetWorld()) >= minPlayersToStart)
	{
		SetRandomPlayerHuntTarget(player);

		if (killer != NULL)
			SetRandomPlayerHuntTarget(killer);
	}
}

void ACMGameMode_Hunt::SetPlayerHuntTarget(APlayerController* player, APlayerController* killer)
{
	// TODO: move huntTarget to PlayerCharacter in order to prevent cheating (every player can read PlayerState)

	ACMPlayerState* playerState = static_cast<ACMPlayerState*>(player->PlayerState);
	ACMPlayerState* killerState = (killer != NULL) ? static_cast<ACMPlayerState*>(killer->PlayerState) : NULL;

	playerState->SetHuntTarget(killer->GetPawn());
	GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, player->GetName() + TEXT(" Hunts ") + killer->GetName());
}

void ACMGameMode_Hunt::SetRandomPlayerHuntTarget(APlayerController* player)
{
	if (GetNumPlayers() < 2)
		return;

	TArray<APlayerController*> players;
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
		players.Add(*iter);

	APlayerController* target = NULL;
	uint32 targetId = 0;

	// randomize target, but disallow self as target
	do
	{
		int j = FMath::RandRange(0, players.Num() - 1);
		target = players[j];
	} while (player == target);

	SetPlayerHuntTarget(player, target);
}