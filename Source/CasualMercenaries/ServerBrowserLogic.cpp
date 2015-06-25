// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "ServerBrowserLogic.h"
#include "NetworkSession.h"



UServerBrowserLogic::UServerBrowserLogic() :Super()
{
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UServerBrowserLogic::OnFindSessionsComplete);
	searchingSessions = false;
}
UServerBrowserLogic::~UServerBrowserLogic()
{
	MarkPendingKill();
}
void UServerBrowserLogic::SetUp(UUserWidget *widget,UWorld *world)
{
	this->widget = widget;
	this->world = world;

	UWidgetTree *widgetTree = widget->WidgetTree;
	TArray<UWidget*> children;
	widgetTree->GetAllWidgets(children);


	int childcount = children.Num();
	for (int i = 0; i < childcount; i++)
	{
		UButton *tempCreateSessionButton = Cast<UButton>(children[i]);
		if (tempCreateSessionButton != nullptr && tempCreateSessionButton->GetName().Equals("CreateSession"))
		{
			createSessionButton = tempCreateSessionButton;
			createSessionButton->OnClicked.AddDynamic(this, &UServerBrowserLogic::CreateSession);
		}
		UScrollBox *tempServerListScrollBox = Cast<UScrollBox>(children[i]);
		if (tempServerListScrollBox != nullptr && tempServerListScrollBox->GetName().Equals("ServerListScrollBox"))
		{
			serverListScrollBox = tempServerListScrollBox;
		}
		UButton *tempFindSessionsButton = Cast<UButton>(children[i]);
		if (tempFindSessionsButton != nullptr && tempFindSessionsButton->GetName().Equals("FindSessionsButton"))
		{
			findSessionsButton = tempFindSessionsButton;
			findSessionsButton->OnClicked.AddDynamic(this, &UServerBrowserLogic::FindSessions);
		}
		UButton *tempAddSession = Cast<UButton>(children[i]);
		if (tempAddSession != nullptr && tempAddSession->GetName().Equals("AddSessionButton"))
		{
			addSessionButton= tempAddSession;
			addSessionButton->OnClicked.AddDynamic(this, &UServerBrowserLogic::AddSessionToGUI);
		}
	}
}
void UServerBrowserLogic::CreateSession()
{
	IOnlineSubsystem *test = IOnlineSubsystem::Get();
	IOnlineSessionPtr session = test->GetSessionInterface();
	FOnlineSessionSettings testSettings;
	testSettings.bAllowInvites = true;
	testSettings.bIsLANMatch = true;
	testSettings.NumPublicConnections = 8;
	testSettings.bShouldAdvertise = true;
	testSettings.bAllowJoinViaPresence = true;

	auto tt = world->GetUniqueID();
	bool succesfull = session->CreateSession(tt, "Game", testSettings);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Did we create session: " + FString::FromInt(succesfull));
}
void UServerBrowserLogic::FindSessions()
{
	if (searchingSessions)
		return;
	IOnlineSubsystem *test = IOnlineSubsystem::Get();
	IOnlineSessionPtr session = test->GetSessionInterface();
	auto tt = world->GetUniqueID();

	SearchSettings = MakeShareable(new FOnlineSessionSearch());
	SearchSettings->MaxSearchResults = 8;
	SearchSettings->bIsLanQuery = true;
	SearchSettings->TimeoutInSeconds = 20;
	TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SearchSettings.ToSharedRef();
	OnFindSessionsCompleteDelegateHandle = session->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
	auto hosted = session->FindSessions(tt, SearchSettingsRef);
	searchingSessions = true;
}
void UServerBrowserLogic::OnFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSubsystem *test = IOnlineSubsystem::Get();
	IOnlineSessionPtr session = test->GetSessionInterface();
	auto tt = world->GetUniqueID();
	searchingSessions = false;
	UE_LOG(LogOnlineGame, Verbose, TEXT("OnFindSessionsComplete bSuccess: %d"), bWasSuccessful);

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Results: " + FString::FromInt(SearchSettings->SearchResults.Num()));
			searchResults = SearchSettings.Get()->SearchResults;
			for (int32 SearchIdx = 0; SearchIdx < SearchSettings->SearchResults.Num(); SearchIdx++)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Test");
				const FOnlineSessionSearchResult& SearchResult = SearchSettings->SearchResults[SearchIdx];
				//SearchResult.Session.SessionInfo.Get()->

				DumpSession(&SearchResult.Session);
			}

			OnFindSessionsComplete().Broadcast(bWasSuccessful);
		}
	}
}
void UServerBrowserLogic::JoinSession()
{
	IOnlineSubsystem *test = IOnlineSubsystem::Get();
	IOnlineSessionPtr session = test->GetSessionInterface();
	auto tt = world->GetUniqueID();
	session->JoinSession(tt, TEXT("Game") , searchResults[0]);
}
void UServerBrowserLogic::AddSessionToGUI()
{
	UButton *newButton = ConstructObject<UButton>(UButton::StaticClass());
	UTextBlock *newText = ConstructObject<UTextBlock>(UTextBlock::StaticClass());
	newText->SetText(FText::FromString("This is server"));
	newButton->AddChild(newText);
	serverListScrollBox->AddChild(newButton);
}