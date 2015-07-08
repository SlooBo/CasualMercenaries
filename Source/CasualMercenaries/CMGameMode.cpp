// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMGameMode.h"
#include "CMPlayerState.h"
#include "PlayerCharacter.h"
#include "Util.h"
#include "PlayerHud.h"
#include "CMPlayerController.h"
#include "CMSpectator.h"
#include "GhostCharacter.h"

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
	PlayerControllerClass = ACMPlayerController::StaticClass();
	PlayerStateClass = ACMPlayerState::StaticClass();
	SpectatorClass = ACMSpectator::StaticClass();
	//GameStateClass = ACMGameState::StaticClass();

	DefaultPlayerName = FText::FromString("OfficeRat");

	bDelayedStart = true;

	startTime = 1;
	warmupTime = 0;

	playerRespawnTime = 2;
	warmupRespawnTime = 1;
	playerRespawnTimeMinimum = -1;
	warmupRespawnTimeMinimum = -1;
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
	AActor* spawnLocation = NULL;

	if (respawnMode == RespawnMode::AtGhost)
	{
		APlayerController* playerController = player->CastToPlayerController();
		if (playerController != NULL)
		{
			spawnLocation = playerController->GetPawnOrSpectator();;
			if (spawnLocation == NULL)
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Error: Unable to get ghost/spectator pawn, choosing best spawn point instead"));
		}
	}
	
	if (respawnMode == RespawnMode::AtSpawnPoint ||
		(respawnMode == RespawnMode::AtGhost && spawnLocation == NULL))
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

		spawnLocation = (AActor*)best;
	}

	return spawnLocation;
}

void ACMGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	inGameState = InGameState::WarmupStarting;

	waitElapsed = -1;
	GetWorld()->GetTimerManager().SetTimer(waitTimer, this, &ACMGameMode::WaitTickSecond, 1.0f, true, 0.0f);
}

void ACMGameMode::StartMatch()
{
	if (inGameState != InGameState::Warmup)
		MatchState = MatchState::WaitingToStart;
	
	Super::StartMatch();

	if (inGameState == InGameState::Warmup)
	{
		OnWarmupStart();
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(waitTimer);
	
	// match needs restarting, move players back to spawn
	for (auto iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		if ((*iter)->GetPawn() != NULL)
			(*iter)->GetPawn()->Reset();

		ACMPlayerState* playerState = Cast<ACMPlayerState>((*iter)->PlayerState);
		if (playerState != NULL)
			playerState->ResetStats();

		RespawnPlayer(*iter);
	}

	mapTimeElapsed = 0;
	GetWorld()->GetTimerManager().SetTimer(mapTimerHandle, this, &ACMGameMode::MapTickSecond, 1.0f, true);

	OnMatchStart();
}

void ACMGameMode::WaitTickSecond()
{
	waitElapsed++;

	if (inGameState == InGameState::WarmupStarting)
	{
		if (waitElapsed >= 0)
		{
			// actually starts warmup and not the real match
			inGameState = InGameState::Warmup;
			StartMatch();
		}
	}

	if (inGameState == InGameState::Warmup)
	{
		int32 numPlayers = GetNumPlayers();

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

	if (inGameState == InGameState::Starting)
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

}

void ACMGameMode::OnPlayerDeath_Implementation(ACMPlayerController* player, ACMPlayerController* killer)
{
	ACMPlayerState* playerState = Cast<ACMPlayerState>(player->PlayerState);
	ACMPlayerState* killerState = (killer != NULL) ? Cast<ACMPlayerState>(killer->PlayerState) : NULL;

	if (playerState != NULL)
		playerState->SetAlive(false);

	if (killerState != NULL)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, playerState->PlayerName + TEXT(" killed ") + killerState->PlayerName);
	else if (killer == player)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, playerState->PlayerName + TEXT(" committed suicide"));
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, playerState->PlayerName + TEXT(" died"));

	// adjust scores
	if (killerState != NULL)
	{
		if (killer != player)
			killerState->AddFrags(1);
		else
			killerState->AddFrags(-1);
	}
	playerState->AddDeaths(1);

	// broadcast death for everyone
	for (auto iter = GetWorld()->GetPlayerControllerIterator(); iter; ++iter)
	{
		ACMPlayerController* playerController = Cast<ACMPlayerController>((*iter).Get());
		playerController->OnPlayerDeath(player, killer);
	}

	SpectatePlayer(player);

	const int32 respawnTime = (inGameState != InGameState::Warmup) ? playerRespawnTime : warmupRespawnTime;

	// deny respawn requests
	if (playerRespawnTimeMinimum != 0)
	{
		denyRespawnList.AddUnique(player);

		// allow manual respawning after minimum period of time
		if (playerRespawnTimeMinimum < respawnTime)
		{
			FTimerHandle timerHandle;
			FTimerDelegate allowDelegate = FTimerDelegate::CreateUObject<ACMGameMode, APlayerController*>(this, &ACMGameMode::AllowPlayerRespawn, player);
			GetWorld()->GetTimerManager().SetTimer(timerHandle, allowDelegate, playerRespawnTimeMinimum, false);
		}
	}

	RespawnPlayer(player, respawnTime);
}

void ACMGameMode::RespawnPlayer(APlayerController* player, float respawnDelay)
{
	if (player == NULL)
		return;

	if (respawnDelay <= 0.0f)
	{
		RestartPlayer(player);
		return;
	}

	
	if (!respawnTimerList.Contains(player))
	{
		FTimerHandle timerHandle;
		respawnTimerList.Add(player, timerHandle);
	}

	FTimerDelegate respawnDelegate = FTimerDelegate::CreateUObject<ACMGameMode, AController*>(this, &ACMGameMode::RestartPlayer, player);
	GetWorld()->GetTimerManager().SetTimer(respawnTimerList[player], respawnDelegate, respawnDelay, false);
}

void ACMGameMode::SpectatePlayer(ACMPlayerController* player)
{
	// disable player pawn instead of destroying it, and let player character destroy itself
	// this fixes some movement related bugs between server and client right after death occured
	if (player->GetPawn() != NULL)
	{
		player->GetPawn()->SetActorHiddenInGame(true);
		player->GetPawn()->SetActorEnableCollision(false);
		player->GetPawn()->SetActorTickEnabled(false);
	}

	// set player to spectator while waiting to respawn
	if (respawnMode != RespawnMode::AtGhost)
	{
		player->PlayerState->bIsSpectator = true;
		player->ChangeState(NAME_Spectating);
		player->ClientGotoState(NAME_Spectating);
	}
	else
	{
		if (player->GetCharacter() != NULL)
		{
			FVector position = player->GetCharacter()->GetActorLocation();
			FRotator rotation = player->GetCharacter()->GetActorRotation();
			rotation.Roll = 0.0f;
			player->UnPossess();

			AGhostCharacter* pawn = GetWorld()->SpawnActor<AGhostCharacter>(AGhostCharacter::StaticClass(), position, rotation);
			player->Possess(pawn);
		}
		//player->ClientSetRotation(NewPlayer->GetCharacter()->GetActorRotation(), true);
		//player->SetControlRotation(NewControllerRot);
	}
}

void ACMGameMode::AllowPlayerRespawn(APlayerController* player)
{
	if (denyRespawnList.Contains(player))
		denyRespawnList.Remove(player);
}

bool ACMGameMode::CanPlayerRespawn(APlayerController* player)
{
	ACMPlayerState* playerState = Cast<ACMPlayerState>(player->PlayerState);
	if (playerState != NULL && !playerState->IsAlive() && !denyRespawnList.Contains(player))
		return true;
	return false;
}

void ACMGameMode::RestartPlayer(AController* controller)
{
	ACMPlayerController* player = Cast<ACMPlayerController>(controller->CastToPlayerController());
	if (player == NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: RestartPlayer controller is not PlayerController"));
		return;
	}

	//clear respawn timers if player respawned early
	if (respawnTimerList.Contains(player))
		GetWorld()->GetTimerManager().ClearTimer(respawnTimerList[player]);

	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(player->GetPawn());
	AGhostCharacter* ghostCharacter = Cast<AGhostCharacter>(player->GetPawn());
	if (playerCharacter != NULL && ghostCharacter == NULL)
		return;

	ACMPlayerState* playerState = Cast<ACMPlayerState>(player->PlayerState);
	if (playerState != NULL)
		playerState->SetAlive(true);

	if (ghostCharacter != NULL)
	{
		ghostCharacter->SetActorHiddenInGame(true);
		ghostCharacter->SetActorEnableCollision(false);
		ghostCharacter->SetActorTickEnabled(false);
	}

	player->PlayerState->bIsSpectator = false;
	player->ChangeState(NAME_Playing);
	player->ClientGotoState(NAME_Playing);

	//Super::RestartPlayer(controller);
	AActor* startPos = FindPlayerStart(player);
	APawn* newPawn = SpawnDefaultPawnFor(player, startPos);

	player->UnPossess();
	player->SetPawn(newPawn);
	player->Possess(newPawn);
	player->ClientSetRotation(newPawn->GetActorRotation(), true);

	FRotator NewControllerRot = startPos->GetActorRotation();
	NewControllerRot.Roll = 0.f;
	player->SetControlRotation(NewControllerRot);

	SetPlayerDefaults(newPawn);

	if (ghostCharacter != NULL)
		ghostCharacter->Destroy();

	player->ServerInitInventory();

	AllowPlayerRespawn(player);
}

void ACMGameMode::SetPlayerDefaults(APawn* playerPawn)
{
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(playerPawn);
	if (playerCharacter == NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: SetPlayerDefaults player pawn is not PlayerCharacter"));
		return;
	}

	ACMPlayerController* player = Cast<ACMPlayerController>(playerCharacter->GetController());
	if (player != NULL)
	{
		//TODO: this should be done when match starts
		ACMPlayerState* playerState = Cast<ACMPlayerState>(player->PlayerState);
		if (playerState != NULL)
		{
			FLinearColor randomColor = FLinearColor::MakeRandomColor();
			playerState->SetColorId(randomColor);
		}

		playerCharacter->ChangeShirtColor(playerState->GetColor(PlayerColor::Shirt));
		//TODO: assign color to character here
	}

	// TODO: setup player character here after respawning (character colors, ...)
}
