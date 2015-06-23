// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMGameMode.h"
#include "CMPlayerState.h"
#include "PlayerCharacter.h"
#include "Util.h"
#include "ChatBroadcaster.h"
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

	chatBroadcaster = CreateDefaultSubobject<UChatBroadcaster>("ChatBroadcaster");
	bDelayedStart = true;

	startTime = 1;
	warmupTime = 0;

	playerRespawnTime = 2;
}


FString ACMGameMode::GetInGameStateAsString(InGameState state)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, STRINGIFY(InGameState), true);
	if (enumPtr == NULL)
		return FString("Invalid");
	return enumPtr->GetEnumName((uint8)state);
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
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, playerState->PlayerName + TEXT(" killed ") + killerState->PlayerName);
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, playerState->PlayerName + TEXT(" died"));

	if (killerState != NULL)
		killerState->AddFrags(1);
	playerState->AddDeaths(1);

	// TODO: broadcast death for every player
	
	// destroy player pawn
	if (player->GetPawn() != NULL)
		player->GetPawn()->Destroy();

	if (inGameState != InGameState::Warmup && playerRespawnTime != 0)
	{
		// delayed player respawn
		if (playerRespawnTime > 0)
		{
			FTimerDelegate respawnDelegate(FTimerDelegate::CreateUObject<ACMGameMode, AController*>(this, &ACMGameMode::RestartPlayer, player));
			GetWorld()->GetTimerManager().SetTimer(respawnDelegate, (float)playerRespawnTime, false);
		}

		// set player to spectator while waiting to respawn
		player->PlayerState->bIsSpectator = true;
		player->ChangeState(NAME_Spectating);
		player->ClientGotoState(NAME_Spectating);
	}
	else	// restart immediately
		RestartPlayer(player);
}

void ACMGameMode::RestartPlayer(AController* controller)
{
	APlayerController* player = controller->CastToPlayerController();
	if (player == NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: RestartPlayer controller is not PlayerController"));
		return;
	}
	
	player->PlayerState->bIsSpectator = false;
	player->ChangeState(NAME_Playing);
	player->ClientGotoState(NAME_Playing);

	Super::RestartPlayer(controller);
}

void ACMGameMode::SetPlayerDefaults(APawn* playerPawn)
{
	APlayerCharacter* playerCharacter = static_cast<APlayerCharacter*>(playerPawn);
	if (playerCharacter == NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: SetPlayerDefaults player pawn is not PlayerCharacter"));
		return;
	}

	// TODO: setup player character here after respawning
}
UChatBroadcaster* ACMGameMode::getServerChat()
{
	return chatBroadcaster;
}