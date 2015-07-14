// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "CMGameMode_Hunt.h"
#include "CMGameMode.h"
#include "CMGameState.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ACMGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ACMGameState(const FObjectInitializer& objectInitializer);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get State Timeleft"), Category = "Gameplay")
	int32 GetStateTimeleft();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get Game Timeleft"), Category = "Gameplay")
	int32 GetGameTimeleft();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get State Time Elapsed"), Category = "Gameplay")
	int32 GetStateTimeElapsed();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get Game Time Elapsed"), Category = "Gameplay")
	int32 GetGameTimeElapsed();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get State Time Length"), Category = "Gameplay")
	int32 GetStateTimeLength();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get Game Time Length"), Category = "Gameplay")
	int32 GetGameTimeLength();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get Base Game State"), Category = "Gameplay")
	InGameState GetBaseGameState() { return inGameState; }

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get Hunt State"), Category = "Gameplay")
	HuntState GetHuntState() { return huntGameState; }

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get Current Hunt Round"), Category = "Gameplay")
	int32 GetHuntCurrentRound() { return huntCurrentRound; }

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get Hunt Rounds Total"), Category = "Gameplay")
	int32 GetHuntTotalRounds() { return huntTotalRounds; }

	// Current state of the base game mode
	UPROPERTY(Replicated)
	InGameState inGameState;

	// Current state of hunt mode
	UPROPERTY(Replicated)
	HuntState huntGameState;

	// Current hunt round number
	UPROPERTY(Replicated)
	int32 huntCurrentRound;

	// Total of hunt rounds in game
	UPROPERTY(Replicated)
	int32 huntTotalRounds;

	// Elapsed time of current game state
	UPROPERTY(Replicated)
	int32 stateTimeElapsed;

	// Length of current game state
	UPROPERTY(Replicated)
	int32 stateTimeLength;

	// Elapsed time of current base game state
	UPROPERTY(Replicated)
	int32 gameTimeElapsed;

	// Length of current base game state
	UPROPERTY(Replicated)
	int32 gameTimeLength;
};
