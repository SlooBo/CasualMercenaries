// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UILogicBase.h"
#include "Online.h"
#include "Engine/GameInstance.h"
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
	UFUNCTION()
	void SetUp(UUserWidget *widget,UWorld *world);
	UFUNCTION()
	void CreateSession();
	UFUNCTION()
	void FindSessions();
	UFUNCTION()
	void JoinSession(int32 searchIndex);
	UFUNCTION()
		void AddSessionToGUI(int32 searchIndex);
	//called from UServerInfo
private:
	TArray<FOnlineSessionSearchResult, FDefaultAllocator> searchResults;
	UPROPERTY()
	bool searchingSessions;
	UPROPERTY()
	UWorld *world;
	UPROPERTY()
	UButton *createSessionButton;
	UPROPERTY()
	UButton *findSessionsButton;
	UPROPERTY()
	UButton *addSessionButton;
	UPROPERTY()
	UButton *connectButton;
	UPROPERTY()
	UScrollBox *serverListScrollBox;
	UPROPERTY()
	UEditableTextBox *ipTextBox;
	UButton *hostGameButton;
	UFUNCTION()
	void ConnectSessionWithIP();
	UFUNCTION()
		void HostGame();
	TSharedPtr<FOnlineSessionSearch> SearchSettings;
	UFUNCTION()
	void OnFindSessionsComplete(bool bWasSuccessful);
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	FOnFindSessionsComplete& OnFindSessionsComplete() { return FindSessionsCompleteEvent; }
	FOnFindSessionsComplete FindSessionsCompleteEvent;
	UPROPERTY()
	TArray<class UServerInfo*> buttonServerInfos;

};
UCLASS()
class UServerInfo: public UObject
{
	GENERATED_BODY()

public:
	
	int32 searchIndex;
	UServerBrowserLogic *serverbrowser;
	UServerInfo()
	{
	}
	UFUNCTION()
		void OnClicked()
	{
		serverbrowser->JoinSession(searchIndex);
	}
};