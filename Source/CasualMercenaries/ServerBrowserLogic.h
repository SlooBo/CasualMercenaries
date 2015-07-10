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
	UUserWidget *widget;
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
	UFUNCTION()
		void ForceGarbageCollector();

	template <typename type>
	bool SetValueFromWidget(type **saveValueHere, FString name)
	{
		UWidgetTree *widgetTree = widget->WidgetTree;
		TArray<UWidget*> children;
		widgetTree->GetAllWidgets(children);

		int childcount = children.Num();
		for (int i = 0; i < childcount; i++)
		{
			type *tempValue = Cast<type>(children[i]);
			UWidget *userWidget = Cast<UWidget>(children[i]);
			if (tempValue != nullptr && userWidget != nullptr && userWidget->GetName().ToLower().Equals(name.ToLower()))
			{
				*saveValueHere = Cast<type>(children[i]);
				return true;
			}
		}
		return false;
	}
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