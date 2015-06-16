// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMGameMode_Hunt.h"
#include "CMPlayerState.h"

ACMGameMode_Hunt::ACMGameMode_Hunt(const class FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	huntMinPlayers = 3;
}

void ACMGameMode_Hunt::StartMatch()
{
	Super::StartMatch();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("GameMode: Hunt"));

	int32 numPlayers = 0;
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
		numPlayers++;

	if (numPlayers >= huntMinPlayers)
	{
		TArray<APlayerController*> players;
		for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
		{
			SetRandomPlayerHuntTarget(*iter);
		}
	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Hunt: Not enough players to start hunt (min: ") + FString::FromInt(huntMinPlayers) + TEXT(")"));
}

void ACMGameMode_Hunt::HandleMatchIsWaitingToStart()
{
	FTimerHandle startTimer;
	GetWorld()->GetTimerManager().SetTimer(startTimer, this, &ACMGameMode_Hunt::StartMatch, 1.0f);
}

void ACMGameMode_Hunt::OnPlayerDeath_Implementation(APlayerController* player, APlayerController* killer)
{
	Super::OnPlayerDeath_Implementation(player, killer);

	SetRandomPlayerHuntTarget(player);
	SetRandomPlayerHuntTarget(killer);
}

void ACMGameMode_Hunt::SetPlayerHuntTarget(APlayerController* player, APlayerController* killer)
{
	// TODO: move huntTarget to PlayerCharacter in order to prevent cheating (every player can read PlayerState)

	ACMPlayerState* playerState = static_cast<ACMPlayerState*>(player->PlayerState);
	ACMPlayerState* killerState = (killer != NULL) ? static_cast<ACMPlayerState*>(killer->PlayerState) : NULL;

	playerState->SetHuntTarget(killer->GetPawn());
}

void ACMGameMode_Hunt::SetRandomPlayerHuntTarget(APlayerController* player)
{
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

	GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, player->GetName() + TEXT(" Hunts ") + target->GetName());
}