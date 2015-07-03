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
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& changedEvent) override;

	static FString GetHuntStateAsString(HuntState state);

	virtual void HandleMatchIsWaitingToStart() override;
	virtual void StartMatch() override;

	// Event when player dies or is killed by other player
	virtual void OnPlayerDeath_Implementation(ACMPlayerController* player, ACMPlayerController* killer = NULL);

	void HuntTickSecond();

	void SetRandomPlayerHuntTarget(ACMPlayerController* player);
	void SetPlayerHuntTarget(ACMPlayerController* player, ACMPlayerController* target);

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

	virtual void OnWarmupStart_Implementation() override;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Rounds", ClampMin = "0"), Category = "Gameplay|Hunt")
	int32 huntRounds;

	// Length of one hunt round
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Round Length", ClampMin = "0"), Category = "Gameplay|Hunt")
	int32 huntRoundTime;

	// Freeze time length at the beginning of a round
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Freeze Time Length", ClampMin = "0"), Category = "Gameplay|Hunt")
	int32 huntRoundFreezeTime;

	// Intermission length between rounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Intermission Length", ClampMin = "0"), Category = "Gameplay|Hunt")
	int32 huntIntermissionTime;

	// Total length of hunt match (calculated from previous values)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (DisplayName = "Hunt Total Length"), Category = "Gameplay|Hunt")
	int32 huntTotalLength;

	//
	//	Money / Kill Rewards
	//

	// Start money for each player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Start Money"), Category = "Gameplay|Hunt|Money")
	int32 huntStartMoney;

	// Limit maximum money players can have
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Max Money"), Category = "Gameplay|Hunt|Money")
	int32 huntMaxMoney;

	// Money given at the end of round
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Round Reward"), Category = "Gameplay|Hunt|Money")
	int32 huntRoundReward;

	// Reward for killing the right target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Kill Reward Target"), Category = "Gameplay|Hunt|Money")
	int32 huntKillRewardTarget;
	
	// Reward for killing a wrong target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Hunt Kill Reward Wrong Target"), Category = "Gameplay|Hunt|Money")
	int32 huntKillRewardWrong;
};
