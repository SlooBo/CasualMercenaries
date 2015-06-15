// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Online.h"
#include "NetworkSession.generated.h"

UCLASS()
class CASUALMERCENARIES_API ANetworkSession : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetworkSession(const FObjectInitializer& ObjectInitializer);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	FOnFindSessionsComplete& OnFindSessionsComplete() { return FindSessionsCompleteEvent; }
	FOnFindSessionsComplete FindSessionsCompleteEvent;
private:
	void OnFindSessionsComplete(bool bWasSuccessful);
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	TSharedPtr<FOnlineSessionSearch> SearchSettings;
	
};
