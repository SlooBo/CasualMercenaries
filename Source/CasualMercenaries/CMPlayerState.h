// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "CMPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ACMPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ACMPlayerState(const FObjectInitializer& objectInitializer);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Reset Player States"), Category = "Gameplay")
	void ResetStats();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get Team Number"), Category = "Gameplay")
	int32 GetTeam();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Set Team Number"), Category = "Gameplay")
	void SetTeam(int32 newTeam);

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get Hunt Target"), Category = "Gameplay|Hunt")
	APawn* GetHuntTarget();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Set Hunt Target"), Category = "Gameplay|Hunt")
	void SetHuntTarget(APawn* target);

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get Frags"), Category = "Gameplay")
	int32 GetFrags();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Add Frags"), Category = "Gameplay")
	void AddFrags(int32 num);

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get Deaths"), Category = "Gameplay")
	int32 GetDeaths();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Add Deaths"), Category = "Gameplay")
	void AddDeaths(int32 num);

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Get Money"), Category = "Gameplay|Hunt")
	int32 GetMoney();

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Add Money"), Category = "Gameplay|Hunt")
	void AddMoney(int32 num);

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "Set Money"), Category = "Gameplay|Hunt")
	void SetMoney(int32 num);

protected:

	// 0 = No Team, 1-4 = Teams
	UPROPERTY(Replicated, EditAnywhere, Meta = (DisplayName = "Team Number", ClampMin = "0", UIMin = "0"))
	int32 team;

	// TODO: move huntTarget to PlayerCharacter in order to prevent cheating (every player can read these)
	// Hunt Target
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Meta = (DisplayName = "Hunt Target"))
	APawn* huntTarget;

	// Player Frags
	UPROPERTY(Replicated, Meta = (DisplayName = "Player Frags"))
	int32 frags;

	// Player Deaths
	UPROPERTY(Replicated, Meta = (DisplayName = "Player Deaths", ClampMin = "0"))
	int32 deaths;

	// Player Money
	UPROPERTY(Replicated, Meta = (DisplayName = "Player Money", ClampMin = "0"))
	int32 money;
};
