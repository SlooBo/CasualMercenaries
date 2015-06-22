// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "CMGameMode.generated.h"

UENUM(BlueprintType)
enum class InGameState : uint8
{
	WaitingForPlayers,
	Warmup,
	Starting,
	Running,
	End,
};

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ACMGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACMGameMode(const class FObjectInitializer& objectInitializer);

	static FString GetInGameStateAsString(InGameState state);

	virtual void HandleMatchIsWaitingToStart() override;
	virtual void StartMatch() override;

	virtual bool ShouldSpawnAtStartSpot(AController* player) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* player) override;
	virtual void SetPlayerDefaults(APawn* playerPawn) override;

	void WaitTickSecond();
	void MapTickSecond();

	virtual void RestartPlayer(AController* controller) override;

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Map Time Left"), Category = "Gameplay|Level")
	int32 MapTimeleft();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Map Time Elapsed"), Category = "Gameplay|Level")
	int32 MapTimeElapsed();

	// Called when warmup starts
	virtual void WarmupStart();

	// Event when match has started
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "On Start Match"), Category = "Gameplay")
	virtual void OnStartMatch();

	// Event when map time has reached zero
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "On Map Timeout"), Category = "Gameplay|Level")
	virtual void OnMapTimeout();

	// Event when player dies or is killed by other player, called by the player
	UFUNCTION(BlueprintNativeEvent, Meta = (DisplayName = "On Player Death"), Category = "Gameplay|Player")
	void OnPlayerDeath(APlayerController* player, APlayerController* killer = NULL);
	virtual void OnPlayerDeath_Implementation(APlayerController* player, APlayerController* killer = NULL);

protected:

	// Timelimit in minutes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Map Timelimit"), Category = "Gameplay|Level")
	int32 mapTimelimit;

	int32 mapTimeElapsed;
	int32 waitElapsed;

	FTimerHandle mapTimerHandle;	// Ticks once every second when the main game mode is running
	FTimerHandle waitTimer;			// Wait for players timer before game starts

	// Internal game state of the game mode
	UPROPERTY(BlueprintReadOnly, Category = Enum)
	InGameState inGameState;
	
	// Start the game mode when at least this many players has joined
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Minimum Players To Start"), Category = "Gameplay|Level")
	int32 minPlayersToStart;

	// Time to wait before game mode starts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Start Delay Time"), Category = "Gameplay|Level")
	int32 startTime;

	// Length of warmup after minimum number of players has joined the level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Warmup Time"), Category = "Gameplay|Level")
	int32 warmupTime;

	// Player respawn time after death (negative values: respawning disabled)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Player Respawn Time"), Category = "Gameplay|Level")
	int32 playerRespawnTime;
};
