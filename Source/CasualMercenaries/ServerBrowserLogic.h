// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UILogicBase.h"
#include "Online.h"
#include "ServerBrowserLogic.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API UServerBrowserLogic : public UUILogicBase
{
	GENERATED_BODY()
	
public:
	UServerBrowserLogic();
	~UServerBrowserLogic();
	void SetUp(UUserWidget *widget,UWorld *world);
	UFUNCTION()
	void CreateSession();
	UFUNCTION()
	void FindSessions();
	UFUNCTION()
	void JoinSession();
	UFUNCTION()
	void AddSessionToGUI();
private:
	TArray<FOnlineSessionSearchResult, FDefaultAllocator> searchResults;
	bool searchingSessions;
	UWorld *world;
	UUserWidget *widget;
	UButton *createSessionButton;
	UButton *findSessionsButton;
	UButton *addSessionButton;
	UScrollBox *serverListScrollBox;
	TSharedPtr<FOnlineSessionSearch> SearchSettings;

	void OnFindSessionsComplete(bool bWasSuccessful);
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	FOnFindSessionsComplete& OnFindSessionsComplete() { return FindSessionsCompleteEvent; }
	FOnFindSessionsComplete FindSessionsCompleteEvent;
};
