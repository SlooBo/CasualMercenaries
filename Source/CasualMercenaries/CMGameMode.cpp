// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMGameMode.h"
#include "CMPlayerState.h"
#include "PlayerCharacter.h"

ACMGameMode::ACMGameMode(const class FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	// defaults for game mode, use blueprints to override

	static ConstructorHelpers::FClassFinder<APawn> playerPawn(TEXT("/Game/Game/Blueprints/BP_PlayerTest"));
	DefaultPawnClass = playerPawn.Class;//APlayerCharacter::StaticClass();

	//HUDClass = ACMHUD::StaticClass();
	//PlayerControllerClass = ACMPlayerController::StaticClass();
	PlayerStateClass = ACMPlayerState::StaticClass();
	//SpectatorClass = ACMSpectatorClass::StaticClass();
	//GameStateClass = ACMGameState::StaticClass();

	DefaultPlayerName = FText::FromString("OfficeRat");
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

void ACMGameMode::StartMatch()
{
	Super::StartMatch();

	GetWorld()->GetTimerManager().ClearTimer(mapTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(mapTimerHandle, this, &ACMGameMode::MapTickSecond, 1.0f, true);

	OnStartMatch();
}

void ACMGameMode::MapTickSecond()
{
	mapTimeElapsed++;
	
	if (mapTimelimit > 0)
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
			// TODO: notify players to display timeleft messages
		}
	}
}

float ACMGameMode::MapTimeleft()
{
	return (mapTimelimit > 0) ? (mapTimelimit - mapTimeElapsed) : 0;
}

float ACMGameMode::MapTimeElapsed()
{
	return mapTimeElapsed;
}

void ACMGameMode::PlayerDeath(APlayerController* player, APlayerController* killer)
{
	OnPlayerDeath(player, killer);
}

void ACMGameMode::OnPlayerDeath_Implementation(APlayerController* player, APlayerController* killer)
{
	ACMPlayerState* playerState = static_cast<ACMPlayerState*>(player->PlayerState);
	ACMPlayerState* killerState = (killer != NULL) ? static_cast<ACMPlayerState*>(killer->PlayerState) : NULL;

	if (killerState != NULL)
		killerState->AddFrags(1);

	playerState->AddDeaths(-1);
	

	if (killer != NULL)
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, player->GetName() + TEXT(" killed ") + killer->GetName());
	else
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, player->GetName() + TEXT(" died"));

	// reset character state to defaults
	player->GetPawn()->Reset();

	Super::RestartPlayer(player);
}