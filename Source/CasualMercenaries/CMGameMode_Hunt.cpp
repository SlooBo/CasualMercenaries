// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMGameMode_Hunt.h"
#include "CMGameState.h"
#include "CMPlayerState.h"
#include "CMPlayerController.h"
#include "PlayerCharacter.h"
#include "DestructibleObject.h"
#include "Util.h"

ACMGameMode_Hunt::ACMGameMode_Hunt(const FObjectInitializer& objectInitializer)
	: ACMGameMode(objectInitializer)
{
	minPlayersToStart = 3;

	huntRounds = 3;
	huntIntermissionLength = 10;
	huntRoundFreezeLength = 3;
	huntRoundLength = 2*60;
	
	huntTotalLength = huntTotalLength = ((huntRoundLength + huntRoundFreezeLength) * huntRounds) + (huntIntermissionLength * (huntRounds - 1));

	playerStartMoney = 5000;
	playerMaxMoney = 20000;
	playerKillRewardTarget = 2000;
	playerKillRewardWrong = -1000;
	huntRoundReward = 5000;

	respawnMode = RespawnMode::AtGhost;
	playerRespawnTime = 20;
	warmupRespawnTime = 1;
	playerRespawnTimeMinimum = 5;
	warmupRespawnTimeMinimum = 0;
}

#if WITH_EDITOR
void ACMGameMode_Hunt::PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent)
{
	if (propertyChangedEvent.Property != NULL)
	{
		FName propertyName = propertyChangedEvent.Property->GetFName();
		if (propertyName == GET_MEMBER_NAME_CHECKED(ACMGameMode_Hunt, huntRounds) ||
			propertyName == GET_MEMBER_NAME_CHECKED(ACMGameMode_Hunt, huntRoundLength) ||
			propertyName == GET_MEMBER_NAME_CHECKED(ACMGameMode_Hunt, huntRoundFreezeLength) ||
			propertyName == GET_MEMBER_NAME_CHECKED(ACMGameMode_Hunt, huntIntermissionLength))
		{
			huntTotalLength = ((huntRoundLength + huntRoundFreezeLength) * huntRounds) + (huntIntermissionLength * (huntRounds - 1));
		}
	}
	
	Super::PostEditChangeProperty(propertyChangedEvent);
}
#endif

FString ACMGameMode_Hunt::GetHuntStateAsString(HuntState state)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, STRINGIFY(HuntState), true);
	if (enumPtr == NULL)
		return FString("Invalid");
	return enumPtr->GetEnumName((uint8)state);
}

void ACMGameMode_Hunt::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
}

void ACMGameMode_Hunt::OnMatchStart_Implementation()
{
	Super::OnMatchStart_Implementation();

	huntElapsed = -1;
	GetWorld()->GetTimerManager().SetTimer(huntTimerHandle, this, &ACMGameMode_Hunt::HuntTickSecond, 1.0f, true, 0.0f);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("GameMode: Hunt"));
}

void ACMGameMode_Hunt::SetupNewPlayer(APlayerController* newPlayer)
{
	Super::SetupNewPlayer(newPlayer);

	ACMPlayerController* pc = Cast<ACMPlayerController>(newPlayer);
	if (pc == NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Error: PlayerController is not CMPlayerController"));
		return;
	}
}

void ACMGameMode_Hunt::HuntTickSecond()
{
	huntElapsed++;
	if (GetHuntTimeLeft() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("End"));
		huntState = HuntState::End;

		//OnHuntEnd();
		huntElapsed = 0;
	}

	// removeme
	if (huntState == HuntState::End)
		huntState = HuntState::Start;

	if (huntState == HuntState::Start)
	{
		huntState = HuntState::Intermission;
		//OnHuntStart();
	}

	//GEngine->AddOnScreenDebugMessage(6765, 3.0f, FColor::Red, FString::FromInt(huntElapsed) + TEXT("/") + FString::FromInt(huntTotalLength) + TEXT(": ") + TEXT("State: ") + GetHuntStateAsString(huntState));

	if (huntState == HuntState::Intermission)
	{
		//GEngine->AddOnScreenDebugMessage(67652, 3.0f, FColor::Green, TEXT("State: ") + GetHuntStateAsString(huntState) + " (" + FString::FromInt(huntIntermissionElapsed) + "s)");
		huntIntermissionElapsed++;

		if (GetHuntIntermissionTimeLeft() <= 5)
			SendAnnouncement("Round is starting in... " + FString::FromInt(GetHuntIntermissionTimeLeft()), 52, 0.5f, 0.17f, 1, FLinearColor::Green);
	}
	else if (huntState == HuntState::Freeze)
	{
		//GEngine->AddOnScreenDebugMessage(67652, 3.0f, FColor::Green, TEXT("State: ") + GetHuntStateAsString(huntState) + " (" + FString::FromInt(huntFreezeTimeElapsed) + "s)");
		huntFreezeTimeElapsed++;
	}
	else if (huntState == HuntState::Round)
	{
		//GEngine->AddOnScreenDebugMessage(67652, 3.0f, FColor::Green, TEXT("State: ") + GetHuntStateAsString(huntState) + " (" + FString::FromInt(huntRoundElapsed) + "s)");
		huntRoundElapsed++;
		if (GetHuntRoundTimeLeft() == 60 || GetHuntRoundTimeLeft() == 30 || GetHuntRoundTimeLeft() == 10 || GetHuntRoundTimeLeft() <= 5)
			SendAnnouncement("Round is ending in... " + FString::FromInt(GetHuntRoundTimeLeft()), 52, 0.5f, 0.17f, (GetHuntRoundTimeLeft() <= 5) ? 1 : 5, FLinearColor::Red);
	}

	HuntState lastState = huntState;
	if (huntIntermissionElapsed >= huntIntermissionLength && huntState == HuntState::Intermission)
	{
		huntIntermissionElapsed = 0;
		huntState = HuntState::Freeze;
		
		if (huntRoundFreezeLength > 0)
			OnRoundFreezeStart();
	}
	if (huntFreezeTimeElapsed >= huntRoundFreezeLength && huntState == HuntState::Freeze)
	{
		huntFreezeTimeElapsed = 0;
		huntState = HuntState::Round;

		if (huntRoundFreezeLength > 0)
			OnRoundFreezeEnd();

		OnRoundStart();
	}
	if (huntRoundElapsed >= huntRoundLength && huntState == HuntState::Round)
	{
		huntRoundElapsed = 0;
		
		huntState = HuntState::Intermission;
		huntCurrentRound++;

		if (lastState == HuntState::Round)
			OnRoundEnd();

		OnIntermissionStart();
	}

	//UpdateGameState();
}

void ACMGameMode_Hunt::UpdateGameState()
{
	Super::UpdateGameState();

	ACMGameState* gameState = Cast<ACMGameState>(GameState);
	if (gameState != NULL)
	{
		gameState->huntGameState = huntState;
		if (huntState == HuntState::Freeze)
		{
			gameState->stateTimeElapsed = huntFreezeTimeElapsed;
			gameState->stateTimeLength = huntRoundFreezeLength;
		}
		else if (huntState == HuntState::Intermission)
		{
			gameState->stateTimeElapsed = huntIntermissionElapsed;
			gameState->stateTimeLength = huntIntermissionLength;
		}
		else
		{
			gameState->stateTimeElapsed = huntRoundElapsed;
			gameState->stateTimeLength = huntRoundLength;
		}

		gameState->gameTimeElapsed = huntElapsed;
		gameState->gameTimeLength = huntTotalLength;
		gameState->huntTotalRounds = huntRounds;
		gameState->huntCurrentRound = huntCurrentRound;
	}
}

void ACMGameMode_Hunt::OnRoundFreezeStart_Implementation()
{
	SendAnnouncement("Round is starting...", 52, 0.5f, 0.17f, huntRoundFreezeLength);
	// freeze players during freeze time
	for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
	{
		APlayerCharacter* pc = Cast<APlayerCharacter>((*iter)->GetCharacter());
		if (pc != NULL && Cast<AGhostCharacter>(pc) == NULL)
			pc->SetState(CHARACTER_STATE::FROZEN);
	}
}

void ACMGameMode_Hunt::OnRoundFreezeEnd_Implementation()
{
	// unfreeze frozen players from frozen state back to unfrozen state
	for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
	{
		APlayerCharacter* pc = Cast<APlayerCharacter>((*iter)->GetCharacter());
		if (pc != NULL && Cast<AGhostCharacter>(pc) == NULL)
			pc->SetState(CHARACTER_STATE::ALIVE);
	}
}

void ACMGameMode_Hunt::OnRoundStart_Implementation()
{
	for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
	{
		// force respawn player
		if ((*iter)->GetPawnOrSpectator() == NULL || Cast<AGhostCharacter>((*iter)->GetPawnOrSpectator()) != NULL)
			RespawnPlayer(*iter);

		SetRandomPlayerHuntTarget(*iter);
		
		// deny shop access from players
		(*iter)->OnShopAccessChanged(false);

		(*iter)->OnRoundStart();
	}

	// restore destructible objects back to their initial state
	for (TActorIterator<ADestructibleObject> iter(GetWorld()); iter; ++iter)
		(*iter)->Respawn();
}

void ACMGameMode_Hunt::OnRoundEnd_Implementation()
{
	for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
	{
		ACMPlayerState* playerState = Cast<ACMPlayerState>((*iter)->PlayerState);
		if (playerState != NULL)
			playerState->AddMoney(huntRoundReward);

		// allow shop access
		(*iter)->OnShopAccessChanged(true);
	}
}

void ACMGameMode_Hunt::OnIntermissionStart_Implementation()
{
	SendAnnouncement("Intermission...", 52, 0.5f, 0.17f, 3);
}

void ACMGameMode_Hunt::OnWarmupStart_Implementation()
{
	for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
	{
		(*iter)->OnWarmupStart();

		// allow shop access
		(*iter)->OnShopAccessChanged(true);
	}
}

void ACMGameMode_Hunt::OnPlayerDeath_Implementation(ACMPlayerController* player, ACMPlayerController* killer)
{
	ACMPlayerState* playerState = Cast<ACMPlayerState>(player->PlayerState);
	ACMPlayerState* killerState = (killer != NULL) ? Cast<ACMPlayerState>(killer->PlayerState) : NULL;
	ACMPlayerController* playerTarget = GetHuntTarget(player);
	ACMPlayerController* killerTarget = GetHuntTarget(killer);

	if (GetNumPlayers() >= minPlayersToStart)
	{
		TArray<ACMPlayerController*> playerHunters;
		for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
		{
			ACMPlayerController* hunterTarget = GetHuntTarget(*iter);
			if (hunterTarget == player)
			{
				//SetPlayerHuntTarget(*iter, NULL);
				playerHunters.Add(*iter);
			}
		}

		if (killer != NULL)
		{
			// player always starts hunting his killer
			SetPlayerHuntTarget(player, killer);

			// hunter(s) new target
			if (playerHunters.Num() < 3)
			{
				if (playerHunters.Num() == 1)
				{
					if (playerTarget != killer)
					{
						// the killer receives killed player's old hunt target
						SetPlayerHuntTarget(killer, playerTarget);
					}
				}
				else if (playerHunters.Num() == 2)
				{
					ACMPlayerController* otherHunter = (killer != playerHunters[0]) ? playerHunters[0] : playerHunters[1];
					if (playerTarget != killer)
					{
						// the killer receives killed player's old hunt target
						SetPlayerHuntTarget(killer, playerTarget);

						// the other hunter starts hunting the killer
						SetPlayerHuntTarget(otherHunter, killer);
					}
				}
				else //if (playerHunters.Num == 0)
				{
					// killer retains his old target if wrong player was killed
				}
				
				if (playerTarget == killer)
				{
					// assign random player as killer's new target, prefering untargeted players
					ACMPlayerController* randomTarget = GetRandomPlayer(killer, player, true);
					SetPlayerHuntTarget(killer, randomTarget);
				}
			}
			else
			{
				// all hunters starts hunting each other, random targets within the group
				for (ACMPlayerController* hunter : playerHunters)
				{
					TArray<ACMPlayerController*> candidates = playerHunters;
					candidates.Remove(hunter);

					ACMPlayerController* randomTarget = Util::RandomFromList(candidates);
					SetPlayerHuntTarget(hunter, randomTarget);
				}
			}
		}
		else
		{
			// player commited suicide
		}

		// sanity check if all players have a target
		for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
		{
			ACMPlayerController* hunterTarget = GetHuntTarget(*iter);
			if (hunterTarget != NULL)
				checkf(hunterTarget != NULL, TEXT("Error: Player has no hunter target, blame Ari"));
		}
	}

	if (killer != NULL)
	{
		// assign new random color for the killed player
		FLinearColor randomColor = FLinearColor::MakeRandomColor();
		if (playerState != NULL)
			playerState->SetColorId(randomColor);
	}

	Super::OnPlayerDeath_Implementation(player, killer);
}

ACMPlayerController* ACMGameMode_Hunt::GetHuntTarget(ACMPlayerController* player)
{
	if (player == NULL)
		return NULL;

	ACMPlayerState* playerState = Cast<ACMPlayerState>(player->PlayerState);
	if (playerState != NULL)
		return Cast<ACMPlayerController>(playerState->GetHuntTarget());

	return NULL;
}

int32 ACMGameMode_Hunt::CountPlayerHunters(ACMPlayerController* player)
{
	int32 hunters = 0;
	for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
	{
		ACMPlayerController* hunterTarget = GetHuntTarget(*iter);
		if (hunterTarget == player)
			hunters++;
	}
	return hunters;
}

ACMPlayerController* ACMGameMode_Hunt::GetRandomPlayer(ACMPlayerController* ignoreTarget, bool preferUnTargeted)
{
	TArray<ACMPlayerController*> ignoreTargets;
	if (ignoreTarget != NULL)
		ignoreTargets.Add(ignoreTarget);
	return GetRandomPlayer(ignoreTargets, preferUnTargeted);
}

ACMPlayerController* ACMGameMode_Hunt::GetRandomPlayer(ACMPlayerController* ignoreTarget1, ACMPlayerController* ignoreTarget2, bool preferUnTargeted)
{
	TArray<ACMPlayerController*> ignoreTargets;
	if (ignoreTarget1 != NULL)
		ignoreTargets.Add(ignoreTarget1);
	if (ignoreTarget2 != NULL)
		ignoreTargets.Add(ignoreTarget2);
	return GetRandomPlayer(ignoreTargets, preferUnTargeted);
}

ACMPlayerController* ACMGameMode_Hunt::GetRandomPlayer(const TArray<ACMPlayerController*>& ignoreTargets, bool preferUnTargeted)
{
	ACMPlayerController* target = NULL;
	TArray<ACMPlayerController*> targetList;

	if (preferUnTargeted)
	{
		for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
		{
			if (!ignoreTargets.Contains(*iter) && CountPlayerHunters(*iter) == 0)
				targetList.Add(*iter);
		}
	}

	if (targetList.Num() == 0)
	{
		for (TActorIterator<ACMPlayerController> iter(GetWorld()); iter; ++iter)
			if (!ignoreTargets.Contains(*iter))
				targetList.Add(*iter);
	}

	if (targetList.Num() != 0)
		target = Util::RandomFromList(targetList);

	return target;
}

void ACMGameMode_Hunt::SetRandomPlayerHuntTarget(ACMPlayerController* player)
{
	ACMPlayerController* target = GetRandomPlayer(player);
	SetPlayerHuntTarget(player, target);
}

void ACMGameMode_Hunt::SetPlayerHuntTarget(ACMPlayerController* player, ACMPlayerController* killer)
{
	// TODO: move huntTarget to PlayerCharacter in order to prevent cheating (every player can read PlayerState)

	ACMPlayerState* playerState = Cast<ACMPlayerState>(player->PlayerState);
	if (playerState != NULL)
		playerState->SetHuntTarget(killer);

	FString killerName = (killer != NULL) ? killer->GetName() : TEXT("NULL");
	//GEngine->AddOnScreenDebugMessage(-1, 12.0f, FColor::Red, player->GetName() + TEXT(" Hunts ") + killerName);
}