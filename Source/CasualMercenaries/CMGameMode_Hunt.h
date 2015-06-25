// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CMGameMode.h"
#include "CMGameMode_Hunt.generated.h"

UENUM(BlueprintType)
enum class HuntState : uint8
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

	static FString GetHuntStateAsString(HuntState state);

	virtual void HandleMatchIsWaitingToStart() override;
	virtual void StartMatch() override;

	// Event when player dies or is killed by other player
	virtual void OnPlayerDeath_Implementation(APlayerController* player, APlayerController* killer = NULL);

	void HuntTickSecond();

	void SetRandomPlayerHuntTarget(APlayerController* player);
	void SetPlayerHuntTarget(APlayerController* player, APlayerController* target);

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Hunt Time Elapsed"), Category = "Gameplay|Level")
	int32 GetHuntTimeElapsed();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Hunt Round Time Elapsed"), Category = "Gameplay|Level")
	int32 GetHuntRoundTimeElapsed();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Hunt Intermission Time Elapsed"), Category = "Gameplay|Level")
	int32 GetHuntIntermissionTimeElapsed();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Hunt Time Left"), Category = "Gameplay|Level")
	int32 GetHuntTimeLeft();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Hunt Round Left"), Category = "Gameplay|Level")
	int32 GetHuntRoundTimeLeft();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Hunt Intermission Left"), Category = "Gameplay|Level")
	int32 GetHuntIntermissionTimeLeft();

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

	int32 GetHuntMaxMoney() { return huntMaxMoney; };

protected:
	FTimerHandle huntTimerHandle;	// Ticks once every second during hunt
	int32 huntElapsed;

	int32 GetHuntTotalLength();
	int32 GetCurrentRound();
	int32 GetNextFreezeStartTime();
	int32 GetNextRoundStartTime();
	int32 GetNextIntermissionStartTime();
	int32 GetNextIntermissionEndTime();

	// Internal game state of hunt mode
	UPROPERTY(BlueprintReadOnly, Category=Enum)
	HuntState huntState;

	//
	//	Round Settings
	//

	// Count of hunt rounds (intermissions = rounds-1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Rounds"), Category = "Gameplay|Hunt")
	int32 huntRounds;

	// Length of one hunt round
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Round Length"), Category = "Gameplay|Hunt")
	int32 huntRoundTime;

	// Freeze time at the beginning of a rounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Freeze Time Length"), Category = "Gameplay|Hunt")
	int32 huntRoundFreezeTime;

	// Intermission length between rounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Intermission Length"), Category = "Gameplay|Hunt")
	int32 huntIntermissionTime;

	//
	//	Money / Kill Rewards
	//

	// Start money for each player
	int32 huntStartMoney;

	// Limit maximum money players can have
	int32 huntMaxMoney;

	// Reward for killing the right target
	int32 huntKillRewardTarget;
	
	// Reward for killing a wrong target
	int32 huntKillRewardWrong;
};
