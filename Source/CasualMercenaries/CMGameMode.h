// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "GhostCharacter.h"
#include "CMGameMode.generated.h"

UENUM(BlueprintType)
enum class InGameState : uint8
{
	WarmupStarting,
	Warmup,
	Starting,
	Running,
	End,
};

UENUM(BlueprintType)
enum class RespawnMode : uint8
{
	// Default mode, spawns at random (team) spawn point
	AtSpawnPoint = 0,

	// Player may spawn at ghost location
	AtGhost,

	// Player may spawn at ghost location, but is placed to random spawn point after death
	AtGhostNearSpawn,
};

UCLASS()
class CASUALMERCENARIES_API ACMGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACMGameMode(const class FObjectInitializer& objectInitializer);

	static FString GetInGameStateAsString(InGameState state);
	InGameState GetInGameState() { return inGameState; };

	virtual void StartNewPlayer(APlayerController* newPlayer) override;
	virtual void Logout(AController* exiting) override;
	virtual void HandleMatchIsWaitingToStart() override;
	virtual void StartMatch() override;

	virtual bool ShouldSpawnAtStartSpot(AController* player) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* player) override;
	virtual AActor* GetRandomSpawnPoint(AController* player);
	virtual void SetPlayerDefaults(APawn* playerPawn) override;
	virtual void RestartPlayer(AController* controller) override;

	virtual void UpdateGameState();

	void WaitTickSecond();
	void MapTickSecond();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Map Time Left"), Category = "Gameplay|Level")
	int32 MapTimeleft();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Map Time Elapsed"), Category = "Gameplay|Level")
	int32 MapTimeElapsed();

	virtual bool CanPlayerRespawn(APlayerController* player);
	void AllowPlayerRespawn(APlayerController* player);
	
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Respawn Player"), Category = "Gameplay|Player")
	void RespawnPlayer(APlayerController* player, float respawnDelay = 0.0f);

	void SpectatePlayer(ACMPlayerController* player);
	
	// Event when warmup has started
	UFUNCTION(BlueprintNativeEvent, Meta = (DisplayName = "On Warmup Start"), Category = "Gameplay")
	void OnWarmupStart();
	virtual void OnWarmupStart_Implementation();

	// Event when match has started
	UFUNCTION(BlueprintNativeEvent, Meta = (DisplayName = "On Match Start"), Category = "Gameplay")
	void OnMatchStart();
	virtual void OnMatchStart_Implementation();

	virtual void SetupNewPlayer(APlayerController* newPlayer);

	// Returns random unused color id for player to use
	FLinearColor GetRandomPlayerColor();

	// Event when map time has reached zero
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "On Map Timeout"), Category = "Gameplay|Level")
	virtual void OnMapTimeout();

	// Event when player dies or is killed by other player, called by the player
	UFUNCTION(BlueprintNativeEvent, Meta = (DisplayName = "On Player Death"), Category = "Gameplay|Player")
	void OnPlayerDeath(ACMPlayerController* player, ACMPlayerController* killer = NULL);
	virtual void OnPlayerDeath_Implementation(ACMPlayerController* player, ACMPlayerController* killer = NULL);

	// Event when player respawns
	UFUNCTION(BlueprintNativeEvent, Meta = (DisplayName = "On Player Respawn"), Category = "Gameplay|Player")
	void OnPlayerRespawn(ACMPlayerController* player);
	virtual void OnPlayerRespawn_Implementation(ACMPlayerController* player);

	int32 GetPlayerMaxMoney() { return playerMaxMoney; };

protected:

	TSubclassOf<class AGhostCharacter> GhostCharacterClass;

	TArray<APlayerController*> denyRespawnList;
	TMap<APlayerController*, FTimerHandle> respawnTimerList;

	// Timelimit in minutes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Map Timelimit", ClampMin = "0"), Category = "Gameplay|Level")
	int32 mapTimelimit;

	int32 mapTimeElapsed;
	int32 waitElapsed;

	FTimerHandle mapTimerHandle;	// Ticks once every second when the main game mode is running
	FTimerHandle waitTimer;			// Wait for players timer before game starts

	// Current state of the game mode
	UPROPERTY(BlueprintReadOnly, Category = Enum)
	InGameState inGameState;

	// Respawning mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Respawn Mode"), Category = Enum)
	RespawnMode respawnMode;
	
	// Start the game mode when at least this many players has joined
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Minimum Players To Start", ClampMin = "0", ClampMax = "64"), Category = "Gameplay|Level")
	int32 minPlayersToStart;

	// Time to wait before game mode starts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Start Delay Time", ClampMin = "0"), Category = "Gameplay|Level")
	int32 startTime;

	// Max Length of warmup while waiting for players to join
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Warmup Time", ClampMin = "0"), Category = "Gameplay|Level")
	int32 warmupTime;

	// Player respawn time after death (-1: respawning disabled)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Player Respawn Time", ClampMin = "-1"), Category = "Gameplay|Level")
	int32 playerRespawnTime;

	// Player respawn time after death during warmup (-1: respawning disabled)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Player Warmup Respawn Time", ClampMin = "-1"), Category = "Gameplay|Level")
	int32 warmupRespawnTime;

	// Minimum player wait time before respawning is allowed (-1: wait until player is respawned by server)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Player Minimum Respawn Time", ClampMin = "-1"), Category = "Gameplay|Level")
	int32 playerRespawnTimeMinimum;

	// Minimum player wait time before respawning is allowed during warmup (-1: wait until player is respawned by server)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Player Minimum Warmup Respawn Time", ClampMin = "-1"), Category = "Gameplay|Level")
	int32 warmupRespawnTimeMinimum;

	//
	//	Money / Kill Rewards
	//

	// Start money for each player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Player Start Money"), Category = "Gameplay|Money")
	int32 playerStartMoney;

	// Limit maximum money players can have
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Player Max Money"), Category = "Gameplay|Money")
	int32 playerMaxMoney;

	// Reward for killing the right target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Player Kill Reward Target"), Category = "Gameplay|Money")
	int32 playerKillRewardTarget;

	// Reward for killing a wrong target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Player Kill Reward Wrong Target"), Category = "Gameplay|Money")
	int32 playerKillRewardWrong;
};
