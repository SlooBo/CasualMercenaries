// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMGameMode.h"
#include "CMGameState.h"
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

	PlayerControllerClass = ACMPlayerController::StaticClass();
	PlayerStateClass = ACMPlayerState::StaticClass();
	SpectatorClass = ACMSpectator::StaticClass();
	GhostCharacterClass = AGhostCharacter::StaticClass();
	GameStateClass = ACMGameState::StaticClass();

	DefaultPlayerName = FText::FromString("OfficeRat");

	bDelayedStart = true;

	startTime = 0;
	warmupTime = 0;

	playerStartMoney = 5000;
	playerMaxMoney = 20000;
	playerKillRewardTarget = 2000;
	playerKillRewardWrong = -1000;

	respawnMode = RespawnMode::AtGhost;
	playerRespawnTime = 15;
	warmupRespawnTime = 1;
	playerRespawnTimeMinimum = 0;
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

	if (respawnMode == RespawnMode::AtGhost || respawnMode == RespawnMode::AtGhostNearSpawn)
	{
		APlayerController* playerController = player->CastToPlayerController();
		if (playerController != NULL)
		{
			spawnLocation = playerController->GetPawnOrSpectator();
			//if (spawnLocation == NULL)
			//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Warning: Unable to get ghost/spectator pawn, choosing best spawn point instead"));
		}
	}
	
	if (respawnMode == RespawnMode::AtSpawnPoint ||
		((respawnMode == RespawnMode::AtGhost || respawnMode == RespawnMode::AtGhostNearSpawn) && spawnLocation == NULL))
	{
		spawnLocation = GetRandomSpawnPoint(player);
		if (spawnLocation == NULL)
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: No spawn points were found, add PlayerStart to level"));
	}

	return spawnLocation;
}

AActor* ACMGameMode::GetRandomSpawnPoint(AController* player)
{
	APlayerStart* best = NULL;
	TArray<APlayerStart*> spawns;

	for (TActorIterator<APlayerStart> iter(GetWorld()); iter; ++iter)
		spawns.Add(*iter);

	// TODO: choose spawn point which is furthest away from any enemy player

	// choose the random spawn
	int random = FMath::RandRange(0, spawns.Num() - 1);
	if (spawns.IsValidIndex(random))
		return spawns[random];

	return NULL;
}

void ACMGameMode::StartNewPlayer(APlayerController* newPlayer)
{
	SetupNewPlayer(newPlayer);

	Super::StartNewPlayer(newPlayer);
}

void ACMGameMode::Logout(AController* exiting)
{
	ACMPlayerController* pc = Cast<ACMPlayerController>(exiting);

	// remove ongoing respawn timers if player disconnects
	if (pc != NULL && respawnTimerList.Contains(pc))
		GetWorld()->GetTimerManager().ClearTimer(respawnTimerList[pc]);

	Super::Logout(exiting);
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
			SendAnnouncement(TEXT("Waiting for more players... ( ") + FString::FromInt(numPlayers) + TEXT("/") + FString::FromInt(minPlayersToStart) + TEXT(" )"), 38, 0.5f, 0.1f, 1, FLinearColor::Red);
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
			SendAnnouncement(TEXT("Warmup, Game starting in... ") + FString::FromInt(warmupTime - waitElapsed), 38, 0.5f, 0.1f, 1, FLinearColor::Green);
		else if (inGameState == InGameState::Starting)
			SendAnnouncement(TEXT("Game is starting in...") + FString::FromInt(startTime - waitElapsed), 38, 0.5f, 0.1f, 1, FLinearColor::Green);
	}

	UpdateGameState();
}

void ACMGameMode::UpdateGameState()
{
	ACMGameState* gameState = Cast<ACMGameState>(GameState);
	if (gameState != NULL)
	{
		gameState->inGameState = inGameState;
		gameState->gameTimeElapsed = mapTimeElapsed;
		gameState->gameTimeLength = mapTimelimit;

		if (inGameState == InGameState::Warmup || inGameState == InGameState::WarmupStarting)
		{
			gameState->stateTimeElapsed = waitElapsed;
			gameState->stateTimeLength = waitElapsed;
			gameState->gameTimeElapsed = waitElapsed;
			gameState->gameTimeLength = warmupTime;
		}
		else if (inGameState == InGameState::Starting)
		{
			gameState->stateTimeElapsed = waitElapsed;
			gameState->stateTimeLength = startTime;
		}
		else
		{
			gameState->stateTimeElapsed = mapTimeElapsed;
			gameState->stateTimeLength = mapTimelimit;
		}
	}
}

void ACMGameMode::MapTickSecond()
{
	mapTimeElapsed++;
	
	if (mapTimelimit > 0 && inGameState != InGameState::Warmup)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("Timeleft: ") + FString::FromInt((int32)MapTimeleft()));
		if (mapTimeElapsed >= mapTimelimit)
		{
			SendAnnouncement(TEXT("Map timelimit reached"), 52, 0.5f, 0.17f, 1, FLinearColor::White);

			mapTimeElapsed = 0;
			OnMapTimeout();
		}
		else
		{
			// TODO: notify players to display timeleft messages?
		}
	}

	UpdateGameState();
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
	for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
	{
		SetupNewPlayer(*iter);
		(*iter)->OnWarmupStart();
	}
}

void ACMGameMode::OnMatchStart_Implementation()
{
	for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
	{
		SetupNewPlayer(*iter);
		(*iter)->OnMatchStart();
	}
}

void ACMGameMode::SetupNewPlayer(APlayerController* newPlayer)
{
	ACMPlayerState* playerState = Cast<ACMPlayerState>(newPlayer->PlayerState);
	if (playerState != NULL)
	{
		if (inGameState == InGameState::Warmup)
			playerState->SetMoney(playerMaxMoney);
		else
			playerState->SetMoney(playerStartMoney);
	}

	ACMPlayerController* pc = Cast<ACMPlayerController>(newPlayer);
	if (pc == NULL)
		return;

	if (pc->GetInventory().weapons.Num() < 1)
		pc->ServerInitInventory();
}

void ACMGameMode::OnPlayerDeath_Implementation(ACMPlayerController* player, ACMPlayerController* killer)
{
	ACMPlayerState* playerState = Cast<ACMPlayerState>(player->PlayerState);
	ACMPlayerState* killerState = (killer != NULL) ? Cast<ACMPlayerState>(killer->PlayerState) : NULL;

	if (playerState == NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: OnPlayerDeath playerState is null"));
		return;
	}

	playerState->SetColorId(FLinearColor::Transparent);
	playerState->SetAlive(false);

	if (killerState != NULL)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, playerState->PlayerName + TEXT(" killed ") + killerState->PlayerName);
	else if (killer == player)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, playerState->PlayerName + TEXT(" committed suicide"));
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, playerState->PlayerName + TEXT(" died"));

	// adjust scores and money
	if (killerState != NULL)
	{
		// negative frag for suicide
		if (killer == player)
			killerState->AddFrags(-1);
		else
			killerState->AddFrags(1);
	}
	playerState->AddDeaths(1);

	player->OnPlayerDeath();

	// broadcast death for everyone
	for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
		(*iter)->OnPlayerDeathBroadcast(player, killer);

	// turn player into spectator
	SpectatePlayer(player);

	const int32 respawnTime = (inGameState != InGameState::Warmup) ? playerRespawnTime : warmupRespawnTime;

	// deny further respawn requests
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

void ACMGameMode::OnPlayerRespawn_Implementation(ACMPlayerController* player)
{
	for (TActorIterator<AGhostCharacter> iter(GetWorld()); iter; ++iter)
		(*iter)->SetPlayerVisibility(false);
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
	if (Cast<AGhostCharacter>(player->GetPawn()) != NULL)
	{
		// reset ghost location if player fell out of level
		AActor* spawnActor = GetRandomSpawnPoint(player);
		if (spawnActor != NULL)
			Cast<AGhostCharacter>(player->GetPawn())->SetActorLocation(spawnActor->GetActorLocation());
		else
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: Could not move Ghost Character"));
		return;
	}

	// disable player pawn instead of destroying it, and let player character destroy itself
	// this fixes some movement related bugs between server and client right after death occured
	if (player->GetPawn() != NULL)
	{
		player->GetPawn()->SetActorHiddenInGame(true);
		player->GetPawn()->SetActorEnableCollision(false);
		player->GetPawn()->SetActorTickEnabled(false);
	}

	// set player to spectator while waiting to respawn
	if (respawnMode == RespawnMode::AtSpawnPoint)
	{
		player->PlayerState->bIsSpectator = true;
		player->ChangeState(NAME_Spectating);
		player->ClientGotoState(NAME_Spectating);
	}
	else
	{
		AActor* spawnActor = player->GetCharacter();
		if (player->GetCharacter() == NULL || respawnMode == RespawnMode::AtGhostNearSpawn)
			spawnActor = GetRandomSpawnPoint(player);
		if (spawnActor == NULL)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: No spawn points were found, add PlayerStart to level"));
			return;
		}

		FVector position = spawnActor->GetActorLocation();
		FRotator rotation = spawnActor->GetActorRotation();
		FRotator newControlRot = rotation;
		if (Cast<APlayerCharacter>(spawnActor) != NULL)
			newControlRot.Pitch = Cast<APlayerCharacter>(spawnActor)->GetControlRotation().Pitch;

		rotation.Roll = 0.0f;
		
		player->UnPossess();
		APawn* pawn = GetWorld()->SpawnActor<APawn>(GhostCharacterClass, position, rotation);
		player->Possess(pawn);

		player->SetControlRotation(newControlRot);

		if (pawn == NULL)
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: Could not spawn Ghost Character, overlapping with another actor?"));
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
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: RestartPlayer, controller is not PlayerController"));
		return;
	}

	// clear respawn timers if player respawned early
	if (respawnTimerList.Contains(player))
		GetWorld()->GetTimerManager().ClearTimer(respawnTimerList[player]);

	if (player->PlayerState == NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: RestartPlayer, PlayerState is null (player disconnected?)"));
		return;
	}

	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(player->GetPawn());
	AGhostCharacter* ghostCharacter = Cast<AGhostCharacter>(player->GetPawn());
	if (playerCharacter != NULL && ghostCharacter == NULL)
	{
		// do nothing if player is already alive
		return;
	}

	// create and place player pawn
	AActor* startPos = FindPlayerStart(player);
	APawn* newPawn = SpawnDefaultPawnFor(player, startPos);
	if (newPawn == NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: SetPlayerDefaults player pawn is not PlayerCharacter"));
		SpectatePlayer(player);
		return;
	}

	if (ghostCharacter != NULL)
	{
		ghostCharacter->SetActorHiddenInGame(true);
		ghostCharacter->SetActorEnableCollision(false);
		ghostCharacter->SetActorTickEnabled(false);
	}

	player->PlayerState->bIsSpectator = false;
	player->ChangeState(NAME_Playing);
	player->ClientGotoState(NAME_Playing);

	FRotator NewControllerRot = startPos->GetActorRotation();
	NewControllerRot.Roll = 0.f;
	if (ghostCharacter != NULL)
		NewControllerRot.Pitch = ghostCharacter->GetControlRotation().Pitch;

	player->UnPossess();
	player->SetPawn(newPawn);
	player->Possess(newPawn);

	player->SetControlRotation(NewControllerRot);

	SetPlayerDefaults(newPawn);

	if (ghostCharacter != NULL)
		ghostCharacter->Destroy();

	AllowPlayerRespawn(player);

	OnPlayerRespawn(player);
}

void ACMGameMode::SetPlayerDefaults(APawn* playerPawn)
{
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(playerPawn);
	if (playerCharacter == NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Error: SetPlayerDefaults player pawn is not PlayerCharacter"));
		return;
	}

	// setup character here
	ACMPlayerController* player = Cast<ACMPlayerController>(playerCharacter->GetController());
	if (player == NULL)
		return;	

	ACMPlayerState* playerState = Cast<ACMPlayerState>(player->PlayerState);
	if (playerState == NULL)
		return;

	if (player != NULL)
	{
		if (playerState != NULL)
			playerCharacter->ChangeShirtColor(playerState->GetColor(PlayerColor::Shirt));
	}

	playerState->SetAlive(true);

	// assign random color for the player
	FLinearColor randomColor = GetRandomPlayerColor();

	playerState->SetColorId(randomColor);
	playerCharacter->ChangeShirtColor(randomColor);

	// reapply all hunt targets to refresh their target colors
	for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
	{
		ACMPlayerState* state = Cast<ACMPlayerState>((*iter)->PlayerState);
		if (state != NULL)
			state->SetHuntTarget(state->GetHuntTarget());
	}
}

FLinearColor ACMGameMode::GetRandomPlayerColor()
{
	static TArray<FLinearColor> colorList;
	if (colorList.Num() == 0)
	{
		// do this once
		// TODO: better colors

		colorList.Add(FLinearColor(1.f, 0.f, 0.f));
		colorList.Add(FLinearColor(0.f, 1.f, 0.f));
		colorList.Add(FLinearColor(0.f, 0.f, 1.f));

		colorList.Add(FLinearColor(1.f, 1.f, 0.f));
		colorList.Add(FLinearColor(0.f, 1.f, 1.f));
		colorList.Add(FLinearColor(1.f, 0.f, 1.f));

		colorList.Add(FLinearColor(1.f, 0.5f, 0.f));
		colorList.Add(FLinearColor(0.f, 1.f, 0.5f));
		colorList.Add(FLinearColor(0.5f, 0.f, 1.f));

		colorList.Add(FLinearColor(1.f, 0.5f, 0.5f));
		colorList.Add(FLinearColor(0.5f, 1.f, 0.5f));
		colorList.Add(FLinearColor(0.5f, 0.5f, 1.f));
	}
	
	TArray<FLinearColor> usedColors;
	for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
	{
		ACMPlayerState* state = Cast<ACMPlayerState>((*iter)->PlayerState);
		if (state != NULL && state->GetColorId() != FLinearColor::Transparent)
			usedColors.Add(state->GetColorId());
	}

	// get unique (unused) but random color
	FLinearColor color = FLinearColor::Transparent;
	for (int i = 0; i < 100; i++)
	{
		color = Util::RandomFromList(colorList);
		if (usedColors.Contains(color))
			continue;
		
		break;
	}
	return color;
}

void ACMGameMode::SendAnnouncement(const FString& announceText, int32 fontsize, float anchorX, float anchorY, int32 lifetime, FLinearColor color/*, USoundCue* announceSoundCue*/)
{
	for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
	{
		(*iter)->OnAnnouncement(announceText, fontsize, anchorX, anchorY, lifetime, color);
	}
}