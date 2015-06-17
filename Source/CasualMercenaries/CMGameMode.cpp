// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMGameMode.h"
#include "CMPlayerState.h"
#include "PlayerCharacter.h"

ACMGameMode::ACMGameMode(const class FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	// defaults for game mode, use blueprints to override

	DefaultPawnClass = APlayerCharacter::StaticClass();
	//HUDClass = ACMHUD::StaticClass();
	//PlayerControllerClass = ACMPlayerController::StaticClass();
	PlayerStateClass = ACMPlayerState::StaticClass();
	//SpectatorClass = ACMSpectatorClass::StaticClass();
	//GameStateClass = ACMGameState::StaticClass();

	DefaultPlayerName = FText::FromString("OfficeRat");

	bDelayedStart = true;

	startTime = 5;
	warmupTime = 10;
}

bool ACMGameMode::ShouldSpawnAtStartSpot(AController* player)
{
	// do not reuse the same spawn point for every respawn
	return false;
}

AActor* ACMGameMode::ChoosePlayerStart_Implementation(AController* player)
{
	APlayerStart* best = NULL;
	TArray<APlayerStart*> spawns;

	for (TActorIterator<APlayerStart> iter(GetWorld()); iter; ++iter)
		spawns.Add(*iter);

	// TODO: choose spawn point which is furthest away from any enemy player

	// choose the random spawn
	int random = FMath::RandRange(0, spawns.Num() - 1);
	if (spawns.IsValidIndex(random))
		best = spawns[random];

	if (best == NULL)
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: No spawn points were found, add PlayerStart to level"));

	return (AActor*)best;
}

void ACMGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	waitElapsed = -1;

	GetWorld()->GetTimerManager().SetTimer(waitTimer, this, &ACMGameMode::WaitTickSecond, 1.0f, true, 0.0f);
}

void ACMGameMode::StartMatch()
{
	Super::StartMatch();

	if (inGameState != InGameState::Warmup)
	{
		GetWorld()->GetTimerManager().ClearTimer(waitTimer);
	
		// match needs restarting, move players back to spawn
		for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
		{
			(*iter)->GetPawn()->Reset();
			Super::RestartPlayer((*iter));
		}
	}

	mapTimeElapsed = 0;
	GetWorld()->GetTimerManager().SetTimer(mapTimerHandle, this, &ACMGameMode::MapTickSecond, 1.0f, true);

	OnStartMatch();
}

void ACMGameMode::WaitTickSecond()
{
	waitElapsed++;

	int32 numPlayers = GetNumPlayers();

	if (numPlayers >= minPlayersToStart && inGameState == InGameState::WaitingForPlayers)
	{
		// ready to start the match or warmup

		waitElapsed = 0;
		if (warmupTime > 0)
		{
			inGameState = InGameState::Warmup;
			WarmupStart();
		}
		else
			inGameState = InGameState::Starting;
	}

	if (inGameState == InGameState::WaitingForPlayers)
	{
		inGameState = InGameState::WaitingForPlayers;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Waiting for more players... ") + FString::FromInt(waitElapsed)
			+ TEXT(" ( ") + FString::FromInt(numPlayers) + TEXT("/") + FString::FromInt(minPlayersToStart) + TEXT(" )"));
	}
	else
	{
		const int32 timeToWait = (inGameState == InGameState::Warmup) ? warmupTime : startTime;
		if (waitElapsed >= timeToWait)
		{
			inGameState = InGameState::Running;
			StartMatch();
		}
		else if (inGameState == InGameState::Warmup)
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("Warmup, Game starting in... ") + FString::FromInt(warmupTime - waitElapsed));
		else if (inGameState == InGameState::Starting)
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("Game starting in... ") + FString::FromInt(startTime - waitElapsed));
	}
}

void ACMGameMode::MapTickSecond()
{
	mapTimeElapsed++;
	
	if (mapTimelimit > 0 && inGameState != InGameState::Warmup)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Timeleft: ") + FString::FromInt((int32)MapTimeleft()));
		if (mapTimeElapsed >= mapTimelimit)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Map timelimit reached"));

			mapTimeElapsed = 0;
			OnMapTimeout();
		}
		else
		{
			// TODO: notify players to display timeleft messages?
		}
	}
}

int32 ACMGameMode::MapTimeleft()
{
	return (mapTimelimit > 0) ? (mapTimelimit - mapTimeElapsed) : 0;
}

int32 ACMGameMode::MapTimeElapsed()
{
	return mapTimeElapsed;
}

void ACMGameMode::WarmupStart()
{
	StartMatch();
}

void ACMGameMode::OnPlayerDeath_Implementation(APlayerController* player, APlayerController* killer)
{
	ACMPlayerState* playerState = static_cast<ACMPlayerState*>(player->PlayerState);
	ACMPlayerState* killerState = (killer != NULL) ? static_cast<ACMPlayerState*>(killer->PlayerState) : NULL;

	if (killerState != NULL)
		killerState->AddFrags(1);

	playerState->AddDeaths(-1);
	
	// TODO: broadcast death for every player

	if (killer != NULL)
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, player->GetName() + TEXT(" killed ") + killer->GetName());
	else
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, player->GetName() + TEXT(" died"));

	// reset character state to defaults
	player->GetPawn()->Reset();
	
	Super::RestartPlayer(player);
}