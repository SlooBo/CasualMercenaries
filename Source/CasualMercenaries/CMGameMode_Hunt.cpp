// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMGameMode_Hunt.h"
#include "CMPlayerState.h"
#include "CMPlayerController.h"
#include "Util.h"

ACMGameMode_Hunt::ACMGameMode_Hunt(const FObjectInitializer& objectInitializer)
	: ACMGameMode(objectInitializer)
{
	minPlayersToStart = 1;

	huntRounds = 2;
	huntRoundTime = 5;
	huntRoundFreezeTime = 2;
	huntIntermissionTime = 3;
	huntTotalLength = ((huntRoundTime + huntRoundFreezeTime) * huntRounds) + (huntIntermissionTime * (huntRounds - 1));

	huntStartMoney = 1500;
	huntMaxMoney = 20000;
	huntKillRewardTarget = 2000;
	huntKillRewardWrong = -1000;

	playerRespawnTimeMinimum = 0;
	warmupRespawnTimeMinimum = 0;
}

void ACMGameMode_Hunt::PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent)
{
	if (propertyChangedEvent.Property != NULL)
	{
		FName propertyName = propertyChangedEvent.Property->GetFName();
		if (propertyName == GET_MEMBER_NAME_CHECKED(ACMGameMode_Hunt, huntRounds) ||
			propertyName == GET_MEMBER_NAME_CHECKED(ACMGameMode_Hunt, huntRoundTime) ||
			propertyName == GET_MEMBER_NAME_CHECKED(ACMGameMode_Hunt, huntRoundFreezeTime) ||
			propertyName == GET_MEMBER_NAME_CHECKED(ACMGameMode_Hunt, huntIntermissionTime))
		{
			huntTotalLength = ((huntRoundTime + huntRoundFreezeTime) * huntRounds) + (huntIntermissionTime * (huntRounds - 1));
		}
	}
	
	Super::PostEditChangeProperty(propertyChangedEvent);
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

	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		ACMPlayerState* playerState = static_cast<ACMPlayerState*>((*iter)->PlayerState);
		if (playerState != NULL)
		{
			if (inGameState != InGameState::Warmup)
			{
				if (Util::GetNumPlayers(GetWorld()) >= minPlayersToStart)
					SetRandomPlayerHuntTarget(static_cast<ACMPlayerController*>((*iter).Get()));

				playerState->SetMoney(huntStartMoney);
			}
			else
				playerState->SetMoney(huntMaxMoney);
		}
	}

	if (inGameState != InGameState::Warmup)
	{
		huntElapsed = -1;
		GetWorld()->GetTimerManager().SetTimer(huntTimerHandle, this, &ACMGameMode_Hunt::HuntTickSecond, 1.0f, true, 0.0f);

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("GameMode: Hunt"));
	}
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
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("End"));
		huntState = HuntState::End;
		huntElapsed = 0;
	}

	GEngine->AddOnScreenDebugMessage(6765, 3.0f, FColor::Red, FString::FromInt(huntElapsed) + TEXT("/") + FString::FromInt(GetHuntTotalLength()) + TEXT(": ") + TEXT("State: ") + GetHuntStateAsString(huntState));

	if (huntElapsed < GetNextRoundStartTime() && huntRoundFreezeTime > 0 && huntState != HuntState::RoundStarting)
	{
		// freeze time started
		huntState = HuntState::RoundStarting;
		OnRoundStart();
		OnRoundFreezeStart();
		GEngine->AddOnScreenDebugMessage(67652, 3.0f, FColor::Green, TEXT("State: ") + GetHuntStateAsString(huntState) + " (" + FString::FromInt(huntRoundFreezeTime) + "s)");
	}
	else if (huntElapsed >= GetNextRoundStartTime() && huntElapsed < GetNextIntermissionStartTime() && huntState != HuntState::Round)
	{
		// round started
		huntState = HuntState::Round;

		if (huntRoundFreezeTime > 0)
			OnRoundFreezeEnd();
		else
			OnRoundStart();

		GEngine->AddOnScreenDebugMessage(67652, 3.0f, FColor::Green, TEXT("State: ") + GetHuntStateAsString(huntState) + " (" + FString::FromInt(huntRoundTime) + "s)");
	}
	else if (huntElapsed >= GetNextIntermissionStartTime() && huntElapsed < GetNextIntermissionEndTime() && huntState != HuntState::Intermission)
	{
		// intermission started
		huntState = HuntState::Intermission;	
		OnIntermissionStart();

		GEngine->AddOnScreenDebugMessage(67652, 3.0f, FColor::Green, TEXT("State: ") + GetHuntStateAsString(huntState) + " (" + FString::FromInt(huntIntermissionTime) + "s)");
	}
}

int32 ACMGameMode_Hunt::GetHuntTimeElapsed()
{
	return huntElapsed;
}

int32 ACMGameMode_Hunt::GetHuntTotalLength()
{
	return huntTotalLength;
}

int32 ACMGameMode_Hunt::GetCurrentRound()
{
	return huntElapsed / (huntRoundFreezeTime + huntRoundTime + huntIntermissionTime);
}

int32 ACMGameMode_Hunt::GetNextFreezeStartTime()
{
	return GetCurrentRound() * (huntRoundFreezeTime + huntRoundTime + huntIntermissionTime);
}

int32 ACMGameMode_Hunt::GetNextRoundStartTime()
{
	return GetNextFreezeStartTime() + huntRoundFreezeTime;
}

int32 ACMGameMode_Hunt::GetNextIntermissionStartTime()
{
	return GetNextRoundStartTime() + huntRoundTime;
}

int32 ACMGameMode_Hunt::GetNextIntermissionEndTime()
{
	return GetNextIntermissionStartTime() + huntIntermissionTime;
}

int32 ACMGameMode_Hunt::GetHuntRoundTimeElapsed()
{
	int32 roundStartTime = GetNextRoundStartTime();

	if (huntElapsed < roundStartTime)
		return -1;

	return huntElapsed - roundStartTime;
}

int32 ACMGameMode_Hunt::GetHuntIntermissionTimeElapsed()
{
	int32 intermissionStartTime = GetNextIntermissionStartTime();

	if (huntElapsed < intermissionStartTime)
		return -1;

	return huntElapsed - intermissionStartTime;
}

int32 ACMGameMode_Hunt::GetHuntTimeLeft()
{
	return GetHuntTotalLength() - huntElapsed;
}

int32 ACMGameMode_Hunt::GetHuntRoundTimeLeft()
{
	if (huntElapsed < GetNextRoundStartTime())
		return -1;

	return GetNextIntermissionStartTime() - huntElapsed;
}

int32 ACMGameMode_Hunt::GetHuntIntermissionTimeLeft()
{
	if (huntElapsed < GetNextIntermissionStartTime())
		return -1;

	return GetNextIntermissionEndTime() - huntElapsed;
}

void ACMGameMode_Hunt::OnRoundFreezeStart_Implementation()
{
	// freeze players during freeze time
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
		(*iter)->SetIgnoreMoveInput(true);
}

void ACMGameMode_Hunt::OnRoundFreezeEnd_Implementation()
{
	// unfreeze frozen players from frozen state back to unfrozen state
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
		(*iter)->SetIgnoreMoveInput(false);
}

void ACMGameMode_Hunt::OnRoundStart_Implementation()
{
	// respawn players
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
		RespawnPlayer(*iter);
}

void ACMGameMode_Hunt::OnIntermissionStart_Implementation()
{
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		ACMPlayerState* playerState = static_cast<ACMPlayerState*>((*iter)->PlayerState);
		if (playerState != NULL)
			playerState->AddMoney(huntRoundReward);
	}
}

void ACMGameMode_Hunt::OnPlayerDeath_Implementation(ACMPlayerController* player, ACMPlayerController* killer)
{
	Super::OnPlayerDeath_Implementation(player, killer);

	if (Util::GetNumPlayers(GetWorld()) >= minPlayersToStart)
	{
		SetRandomPlayerHuntTarget(player);

		if (killer != NULL)
		{
			ACMPlayerState* killerState = static_cast<ACMPlayerState*>(killer->PlayerState);
			ACMPlayerController* killerTarget = NULL;

			if (killerState->GetHuntTarget() != NULL && killerState->GetHuntTarget()->GetNetOwningPlayer() != NULL)
				killerTarget = static_cast<ACMPlayerController*>(killerState->GetHuntTarget()->GetNetOwningPlayer()->PlayerController);

			// killer killed their target?
			if (killerTarget == player)
			{
				killerState->AddMoney(huntKillRewardTarget);
				SetRandomPlayerHuntTarget(killer);
			}
			else if (killerTarget != NULL)
			{
				killerState->AddMoney(huntKillRewardWrong);
			}
		}
	}
}

void ACMGameMode_Hunt::SetPlayerHuntTarget(ACMPlayerController* player, ACMPlayerController* killer)
{
	// TODO: move huntTarget to PlayerCharacter in order to prevent cheating (every player can read PlayerState)

	ACMPlayerState* playerState = static_cast<ACMPlayerState*>(player->PlayerState);

	playerState->SetHuntTarget(killer->GetPawn());
	GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, player->GetName() + TEXT(" Hunts ") + killer->GetName());
}

void ACMGameMode_Hunt::SetRandomPlayerHuntTarget(ACMPlayerController* player)
{
	if (GetNumPlayers() < 2)
		return;

	TArray<ACMPlayerController*> players;
	for (auto iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
		players.Add(static_cast<ACMPlayerController*>((*iter).Get()));

	ACMPlayerController* target = NULL;
	uint32 targetId = 0;

	// randomize target, but disallow self as target
	do
	{
		int j = FMath::RandRange(0, players.Num() - 1);
		target = players[j];
	} while (player == target);

	SetPlayerHuntTarget(player, target);
}