// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CMGameMode.h"
#include "CMGameMode_Hunt.generated.h"

UENUM(BlueprintType)
enum class HuntState : uint8
{
	Start = 0,
	Intermission,
	Freeze,
	Round,
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

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& changedEvent) override;
#endif

	static FString GetHuntStateAsString(HuntState state);

	virtual void HandleMatchIsWaitingToStart() override;
	virtual void OnMatchStart_Implementation() override;

	virtual void SetupNewPlayer(APlayerController* newPlayer) override;

	// Event when player dies or is killed by other player
	virtual void OnPlayerDeath_Implementation(ACMPlayerController* player, ACMPlayerController* killer = NULL);

	void HuntTickSecond();

	void SetRandomPlayerHuntTarget(ACMPlayerController* player);
	void SetPlayerHuntTarget(ACMPlayerController* player, ACMPlayerController* target);

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Hunt Time Elapsed"), Category = "Gameplay|Level")
	int32 GetHuntTimeElapsed() { return huntElapsed; }

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Hunt Round Time Elapsed"), Category = "Gameplay|Level")
	int32 GetHuntRoundElapsed() { return huntRoundElapsed; }

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Hunt Intermission Time Elapsed"), Category = "Gameplay|Level")
	int32 GetHuntIntermissionElapsed() { return huntIntermissionElapsed; }

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Hunt Time Left"), Category = "Gameplay|Level")
	int32 GetHuntTimeLeft() { return huntTotalLength - huntElapsed; }

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Hunt Round Time Left"), Category = "Gameplay|Level")
	int32 GetHuntRoundTimeLeft() { return huntRoundLength - huntRoundElapsed; }

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Hunt Intermission Time Left"), Category = "Gameplay|Level")
	int32 GetHuntIntermissionTimeLeft() { return huntIntermissionLength - huntIntermissionElapsed; }

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Hunt Current Round"), Category = "Gameplay|Level")
	int32 GetCurrentRound() { return huntCurrentRound; }

	// Event when round freeze period starts
	UFUNCTION(BlueprintNativeEvent, Meta = (DisplayName = "On Round Freeze Start"), Category = "Gameplay")
	void OnRoundFreezeStart();

	// Event when round freeze period ends
	UFUNCTION(BlueprintNativeEvent, Meta = (DisplayName = "On Round Freeze End"), Category = "Gameplay")
	void OnRoundFreezeEnd();

	// Event when round starts
	UFUNCTION(BlueprintNativeEvent, Meta = (DisplayName = "On Round Start"), Category = "Gameplay")
	void OnRoundStart();

	// Event after round ends
	UFUNCTION(BlueprintNativeEvent, Meta = (DisplayName = "On Intermission Start"), Category = "Gameplay")
	void OnIntermissionStart();

	virtual void OnWarmupStart_Implementation() override;

protected:
	FTimerHandle huntTimerHandle;	// Ticks once every second during hunt
	int32 huntElapsed;
	int32 huntCurrentRound;
	int32 huntIntermissionElapsed;
	int32 huntFreezeTimeElapsed;
	int32 huntRoundElapsed;

	// Internal game state of hunt mode
	UPROPERTY(BlueprintReadOnly, Category=Enum)
	HuntState huntState;

	//
	//	Round Settings
	//

	// Count of hunt rounds (intermissions = rounds-1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Rounds", ClampMin = "0"), Category = "Gameplay|Hunt")
	int32 huntRounds;

	// Length of one hunt round
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Round Length", ClampMin = "0"), Category = "Gameplay|Hunt")
	int32 huntRoundLength;

	// Freeze time length at the beginning of a round
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Freeze Time Length", ClampMin = "0"), Category = "Gameplay|Hunt")
	int32 huntRoundFreezeLength;

	// Intermission length between rounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Intermission Length", ClampMin = "0"), Category = "Gameplay|Hunt")
	int32 huntIntermissionLength;

	// Total length of hunt match (calculated from previous values)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (DisplayName = "Hunt Total Length"), Category = "Gameplay|Hunt")
	int32 huntTotalLength;

	//
	//	Hunt Money / Kill Rewards
	//

	// Money given at the end of round
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Round Reward"), Category = "Gameplay|Hunt")
	int32 huntRoundReward;
};
