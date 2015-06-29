// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMGameMode.h"
#include "CMPlayerState.h"
#include "PlayerCharacter.h"
#include "Util.h"
#include "PlayerHud.h"

ACMGameMode::ACMGameMode(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	// defaults for game mode, use blueprints to override

	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerCharacter(TEXT("Blueprint'/Game/Game/Blueprints/BP_PlayerCharacter.BP_PlayerCharacter'"));
	if (PlayerCharacter.Object)
		DefaultPawnClass = (UClass*)PlayerCharacter.Object->GeneratedClass;

	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerHud(TEXT("Blueprint'/Game/Game/Blueprints/BP_PlayerHud.BP_PlayerHud'"));
	if (PlayerHud.Object)
		HUDClass = (UClass*)PlayerHud.Object->GeneratedClass;

	//HUDClass = APlayerHud::StaticClass();
	//PlayerControllerClass = ACMPlayerController::StaticClass();
	PlayerStateClass = ACMPlayerState::StaticClass();
	//SpectatorClass = ACMSpectatorClass::StaticClass();
	//GameStateClass = ACMGameState::StaticClass();

	DefaultPlayerName = FText::FromString("OfficeRat");

	bDelayedStart = true;

	startTime = 1;
	warmupTime = 0;

	playerRespawnTime = 2;
	warmupRespawnTime = 1;
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

	inGameState = InGameState::Warmup;
	OnWarmupStart();

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
			if ((*iter)->GetPawn() != NULL)
				(*iter)->GetPawn()->Reset();

			ACMPlayerState* playerState = static_cast<ACMPlayerState*>((*iter)->PlayerState);
			if (playerState != NULL)
				playerState->ResetStats();

			RespawnPlayer(*iter);
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

	if (inGameState == InGameState::Warmup)
	{
		// ready to start the match or warmup
		if (numPlayers >= minPlayersToStart)
		{
			waitElapsed = 0;
			inGameState = InGameState::Starting;
		}
		else if (warmupTime != 0 && waitElapsed >= warmupTime)
		{
			// TODO: what happens when warmup ends and minimum number of players are not met?
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Waiting for more players... ") + FString::FromInt(waitElapsed)
				+ TEXT(" ( ") + FString::FromInt(numPlayers) + TEXT("/") + FString::FromInt(minPlayersToStart) + TEXT(" )"));
		}
	}
	else if (inGameState == InGameState::Starting)
	{
		if (waitElapsed >= startTime)
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

void ACMGameMode::OnWarmupStart_Implementation()
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
	
	// disable player pawn instead of destroying it, and let player character destroy itself
	// this fixes some movement related bugs between server and client right after death occured
	if (player->GetPawn() != NULL)
	{
		player->GetPawn()->SetActorHiddenInGame(true);
		player->GetPawn()->SetActorEnableCollision(false);
		player->GetPawn()->SetActorTickEnabled(false);
	}

	// set player to spectator while waiting to respawn
	player->PlayerState->bIsSpectator = true;
	player->ChangeState(NAME_Spectating);
	player->ClientGotoState(NAME_Spectating);

	if (inGameState != InGameState::Warmup && playerRespawnTime != 0)
		RespawnPlayer(player, playerRespawnTime);
	else	// restart immediately
		RespawnPlayer(player, warmupRespawnTime);
}

void ACMGameMode::RespawnPlayer(APlayerController* player, float respawnDelay)
{
	if (respawnDelay <= 0.0f)
	{
		RestartPlayer(player);
		return;
	}

	FTimerHandle timerHandle;
	FTimerDelegate respawnDelegate = FTimerDelegate::CreateUObject<ACMGameMode, AController*>(this, &ACMGameMode::RestartPlayer, player);
	GetWorld()->GetTimerManager().SetTimer(timerHandle, respawnDelegate, respawnDelay, false);
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
