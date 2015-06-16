// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CMGameMode.h"
#include "CMGameMode_Hunt.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ACMGameMode_Hunt : public ACMGameMode
{
	GENERATED_BODY()

public:
	ACMGameMode_Hunt(const class FObjectInitializer& objectInitializer);

	virtual void StartMatch() override;
	virtual void HandleMatchIsWaitingToStart() override;

	// Event when player dies or is killed by other player
	virtual void OnPlayerDeath_Implementation(APlayerController* player, APlayerController* killer = NULL);

	void SetRandomPlayerHuntTarget(APlayerController* player);
	void SetPlayerHuntTarget(APlayerController* player, APlayerController* target);

protected:
	int32 huntMinPlayers;
};
