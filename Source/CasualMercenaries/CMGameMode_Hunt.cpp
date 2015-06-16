// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMGameMode_Hunt.h"
#include "CMPlayerState.h"
#include "Util.h"

ACMGameMode_Hunt::ACMGameMode_Hunt(const class FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	minPlayersToStart = 3;
}

void ACMGameMode_Hunt::StartMatch()
{
	Super::StartMatch();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("GameMode: Hunt"));

	TArray<APlayerController*> players;
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		if (Util::GetNumPlayers(GetWorld()) >= minPlayersToStart)
			SetRandomPlayerHuntTarget(*iter);
	}
}

void ACMGameMode_Hunt::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
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