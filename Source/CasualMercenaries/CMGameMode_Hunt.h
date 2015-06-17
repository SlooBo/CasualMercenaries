// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CMGameMode.h"
#include "CMGameMode_Hunt.generated.h"

enum class HuntState
{
	Start = 0,
	RoundStarting,	// during freeze time
	Round,
	Intermission,
	End,
};


/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ACMGameMode_Hunt : public ACMGameMode
{
	GENERATED_BODY()

public:
	ACMGameMode_Hunt(const class FObjectInitializer& objectInitializer);

	virtual void HandleMatchIsWaitingToStart() override;
	virtual void StartMatch() override;

	// Event when player dies or is killed by other player
	virtual void OnPlayerDeath_Implementation(APlayerController* player, APlayerController* killer = NULL);

	void HuntTickSecond();

	void SetRandomPlayerHuntTarget(APlayerController* player);
	void SetPlayerHuntTarget(APlayerController* player, APlayerController* target);

	int32 GetHuntTimeElapsed();
	int32 GetHuntRoundTimeElapsed();
	int32 GetHuntIntermissionTimeElapsed();

	int32 GetHuntTimeLeft();
	int32 GetHuntRoundTimeLeft();
	int32 GetHuntIntermissionTimeLeft();

	void RoundFreezeStart();
	void RoundStart();
	void IntermissionStart();

protected:
	FTimerHandle huntTimerHandle;	// Ticks once every second during hunt
	int32 huntElapsed;

	HuntState huntState;			// Internal game state of hunt mode
	int32 huntRounds;				// Count of hunt rounds (intermissions = rounds-1)
	int32 huntRoundTime;			// Length of one hunt round
	int32 huntRoundFreezeTime;		// Freeze time at the beginning of a rounds
	int32 huntIntermissionTime;		// Intermission length between rounds
};
