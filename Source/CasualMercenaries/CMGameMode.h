// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "CMGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ACMGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACMGameMode(const class FObjectInitializer& objectInitializer);

	virtual void StartMatch() override;

	virtual bool ShouldSpawnAtStartSpot(AController* player) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* player) override;

	void MapTickSecond();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Map Time Left"), Category = "Gameplay|Level")
	float MapTimeleft();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Map Time Elapsed"), Category = "Gameplay|Level")
	float MapTimeElapsed();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Player Death"), Category = "Gameplay|Player")
	void PlayerDeath(APlayerController* player, APlayerController* killer = NULL);

	// Event when match has started
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "On Start Match"), Category = "Gameplay")
	virtual void OnStartMatch();

	// Event when map time has reached zero
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "On Map Timeout"), Category = "Gameplay|Level")
	virtual void OnMapTimeout();

	// Event when player dies or is killed by other player
	UFUNCTION(BlueprintNativeEvent, Meta = (DisplayName = "On Player Death"), Category = "Gameplay|Player")
	void OnPlayerDeath(APlayerController* player, APlayerController* killer = NULL);
	virtual void OnPlayerDeath_Implementation(APlayerController* player, APlayerController* killer = NULL);

protected:
	// Timelimit in minutes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Map Timelimit"))
	int32 mapTimelimit;

	FTimerHandle mapTimerHandle;
	int32 mapTimeElapsed;
};
