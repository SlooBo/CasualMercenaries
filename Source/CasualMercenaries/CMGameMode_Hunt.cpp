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

	huntRounds = 3;
	huntIntermissionLength = 4;
	huntRoundFreezeLength = 1;
	huntRoundLength = 10;
	
	huntTotalLength = huntTotalLength = ((huntRoundLength + huntRoundFreezeLength) * huntRounds) + (huntIntermissionLength * (huntRounds - 1));

	playerStartMoney = 5000;
	playerMaxMoney = 20000;
	playerKillRewardTarget = 2000;
	playerKillRewardWrong = -1000;

	playerRespawnTimeMinimum = 0;
	warmupRespawnTimeMinimum = 0;
}

void ACMGameMode_Hunt::PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent)
{
	if (propertyChangedEvent.Property != NULL)
	{
		FName propertyName = propertyChangedEvent.Property->GetFName();
		if (propertyName == GET_MEMBER_NAME_CHECKED(ACMGameMode_Hunt, huntRounds) ||
			propertyName == GET_MEMBER_NAME_CHECKED(ACMGameMode_Hunt, huntRoundLength) ||
			propertyName == GET_MEMBER_NAME_CHECKED(ACMGameMode_Hunt, huntRoundFreezeLength) ||
			propertyName == GET_MEMBER_NAME_CHECKED(ACMGameMode_Hunt, huntIntermissionLength))
		{
			huntTotalLength = ((huntRoundLength + huntRoundFreezeLength) * huntRounds) + (huntIntermissionLength * (huntRounds - 1));
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
		ACMPlayerState* playerState = Cast<ACMPlayerState>((*iter)->PlayerState);
		if (playerState != NULL)
			if (inGameState != InGameState::Warmup && Util::GetNumPlayers(GetWorld()) >= minPlayersToStart)
				SetRandomPlayerHuntTarget(Cast<ACMPlayerController>((*iter).Get()));
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

		//OnHuntEnd();
		huntElapsed = 0;
	}

	// removeme
	if (huntState == HuntState::End)
		huntState = HuntState::Start;

	if (huntState == HuntState::Start)
	{
		huntState = HuntState::Intermission;
		//OnHuntStart();
	}

	GEngine->AddOnScreenDebugMessage(6765, 3.0f, FColor::Red, FString::FromInt(huntElapsed) + TEXT("/") + FString::FromInt(huntTotalLength) + TEXT(": ") + TEXT("State: ") + GetHuntStateAsString(huntState));

	if (huntState == HuntState::Intermission)
	{
		GEngine->AddOnScreenDebugMessage(67652, 3.0f, FColor::Green, TEXT("State: ") + GetHuntStateAsString(huntState) + " (" + FString::FromInt(huntIntermissionElapsed) + "s)");
		huntIntermissionElapsed++;
	}
	else if (huntState == HuntState::Freeze)
	{
		GEngine->AddOnScreenDebugMessage(67652, 3.0f, FColor::Green, TEXT("State: ") + GetHuntStateAsString(huntState) + " (" + FString::FromInt(huntFreezeTimeElapsed) + "s)");
		huntFreezeTimeElapsed++;
	}
	else if (huntState == HuntState::Round)
	{
		GEngine->AddOnScreenDebugMessage(67652, 3.0f, FColor::Green, TEXT("State: ") + GetHuntStateAsString(huntState) + " (" + FString::FromInt(huntRoundElapsed) + "s)");
		huntRoundElapsed++;
	}

	

	if (huntIntermissionElapsed > huntIntermissionLength)
	{
		huntIntermissionElapsed = 0;
		huntState = HuntState::Freeze;

		if (huntRoundFreezeLength > 0)
			OnRoundFreezeStart();
	}
	if (huntFreezeTimeElapsed > huntRoundFreezeLength)
	{
		huntFreezeTimeElapsed = 0;
		huntState = HuntState::Round;

		if (huntRoundFreezeLength > 0)
			OnRoundFreezeEnd();

		OnRoundStart();
	}
	if (huntRoundElapsed > huntRoundLength)
	{
		huntRoundElapsed = 0;
		huntState = HuntState::Intermission;
		huntCurrentRound++;

		OnIntermissionStart();
	}
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
	{
		ACMPlayerController* player = Cast<ACMPlayerController>((*iter).Get());

		// deny shop access from players
		player->OnShopAccessChanged(false);
	}
}

void ACMGameMode_Hunt::OnIntermissionStart_Implementation()
{
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		ACMPlayerController* player = Cast<ACMPlayerController>((*iter).Get());
		ACMPlayerState* playerState = Cast<ACMPlayerState>((*iter)->PlayerState);
		if (playerState != NULL)
			playerState->AddMoney(huntRoundReward);

		// allow shop access
		player->OnShopAccessChanged(true);
	}
}

void ACMGameMode_Hunt::OnWarmupStart_Implementation()
{
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		ACMPlayerController* player = Cast<ACMPlayerController>((*iter).Get());

		// allow shop access
		player->OnShopAccessChanged(true);
	}
}

void ACMGameMode_Hunt::OnPlayerDeath_Implementation(ACMPlayerController* player, ACMPlayerController* killer)
{
	Super::OnPlayerDeath_Implementation(player, killer);

	if (Util::GetNumPlayers(GetWorld()) >= minPlayersToStart)
	{
		ACMPlayerState* playerState = Cast<ACMPlayerState>(player->PlayerState);
		SetRandomPlayerHuntTarget(player);

		if (killer != NULL)
		{
			ACMPlayerState* killerState = Cast<ACMPlayerState>(killer->PlayerState);
			ACMPlayerController* killerTarget = NULL;

			if (killerState->GetHuntTarget() != NULL && killerState->GetHuntTarget()->GetNetOwningPlayer() != NULL)
				killerTarget = Cast<ACMPlayerController>(killerState->GetHuntTarget()->GetNetOwningPlayer()->PlayerController);

			// give new target if correct target was killed
			if (killerTarget == player)
				SetRandomPlayerHuntTarget(killer);
		}

		// assign new random color for the player
		FLinearColor randomColor = FLinearColor::MakeRandomColor();
		if (playerState != NULL)
			playerState->SetColorId(randomColor);
	}
}

void ACMGameMode_Hunt::SetPlayerHuntTarget(ACMPlayerController* player, ACMPlayerController* killer)
{
	// TODO: move huntTarget to PlayerCharacter in order to prevent cheating (every player can read PlayerState)

	ACMPlayerState* playerState = Cast<ACMPlayerState>(player->PlayerState);

	playerState->SetHuntTarget(killer->GetPawn());
	GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, player->GetName() + TEXT(" Hunts ") + killer->GetName());
}

void ACMGameMode_Hunt::SetRandomPlayerHuntTarget(ACMPlayerController* player)
{
	if (GetNumPlayers() < 2)
		return;

	TArray<ACMPlayerController*> players;
	for (auto iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
		players.Add(Cast<ACMPlayerController>((*iter).Get()));

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