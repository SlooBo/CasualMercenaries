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
}
void UServerBrowserLogic::SetUp(UUserWidget *widget,UWorld *world)
{
	this->widget = widget;
	this->world = world;

	SetValueFromWidget(&createSessionButton, "CreateSession");
	createSessionButton->OnClicked.AddDynamic(this, &UServerBrowserLogic::CreateSession);

	SetValueFromWidget(&serverListScrollBox, "ServerListScrollBox");

	SetValueFromWidget(&findSessionsButton, "FindSessionsButton");
	findSessionsButton->OnClicked.AddDynamic(this, &UServerBrowserLogic::FindSessions);


	UButton *tempDestroyButton = nullptr;
	SetValueFromWidget(&tempDestroyButton, "DestroyWorld");
	tempDestroyButton->OnClicked.AddDynamic(this, &UServerBrowserLogic::ForceGarbageCollector);

	SetValueFromWidget(&connectButton, "ConnectButton");
	connectButton->OnClicked.AddDynamic(this, &UServerBrowserLogic::ConnectSessionWithIP);

	SetValueFromWidget(&ipTextBox, "IPTextBox");

	SetValueFromWidget(&hostGameButton, "HostButton");
	hostGameButton->OnClicked.AddDynamic(this, &UServerBrowserLogic::HostGame);


		/*
		tempCreateSessionButton = nullptr;
		tempServerListScrollBox = nullptr;
		tempFindSessionsButton = nullptr;
		tempDestroyButton = nullptr;
		tempConnectButton = nullptr;*/
		/*
		UButton *tempAddSession = Cast<UButton>(children[i]);
		if (tempAddSession != nullptr && tempAddSession->GetName().Equals("AddSessionButton"))
		{
			addSessionButton= tempAddSession;
			addSessionButton->OnClicked.AddDynamic(this, &UServerBrowserLogic::AddSessionToGUI);

		}*/
	
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
				//auto  autoni = SearchSettings->SearchResults[0];
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Test");
				const FOnlineSessionSearchResult& SearchResult = SearchSettings->SearchResults[SearchIdx];
				AddSessionToGUI(SearchIdx);
				//SearchResult.Session.SessionInfo.Get()->

				DumpSession(&SearchResult.Session);
			}

			OnFindSessionsComplete().Broadcast(bWasSuccessful);
		}
	}
}
void UServerBrowserLogic::JoinSession(int32 searchIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "JOINING SESSION!");
	IOnlineSubsystem *test = IOnlineSubsystem::Get();
	IOnlineSessionPtr session = test->GetSessionInterface();
	auto tt = world->GetUniqueID();
	session->JoinSession(tt, TEXT("Game") , searchResults[searchIndex]);
}
void UServerBrowserLogic::AddSessionToGUI(int32 searchIndex)
{
	UPROPERTY()
	UButton *newButton = NewObject<UButton>(UButton::StaticClass());
	UPROPERTY()
	UTextBlock *newText = NewObject<UTextBlock>(UTextBlock::StaticClass());
	newText->SetText(FText::FromString("This is server"));
	newButton->AddChild(newText);
	serverListScrollBox->AddChild(newButton);

	UPROPERTY()
	UServerInfo* serverInfo = NewObject<UServerInfo>();
	serverInfo->searchIndex = searchIndex;
	serverInfo->serverbrowser = this;
	newButton->OnClicked.AddDynamic(serverInfo, &UServerInfo::OnClicked);
	buttonServerInfos.Add(serverInfo);
	
	//serverInfo->SetFlags(RF_RootSet);
	UWidgetTree *widgetTree = widget->WidgetTree;
	TArray<UWidget*> children;
	widgetTree->GetAllWidgets(children);


	int childcount = children.Num();
	for (int i = 0; i < childcount; i++)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Child name: " + children[i]->GetName());

	}
}
void UServerBrowserLogic::ForceGarbageCollector()
{
	world->ForceGarbageCollection(true);
}
void UServerBrowserLogic::ConnectSessionWithIP()
{
	if (ipTextBox == nullptr)
		return;
	FString ipText = ipTextBox->GetText().ToString();
	if (ipText.IsEmpty() || !ipText.Contains("."))
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Invalid ip: " + ipText);
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Connected with: " + ipText);
	world->GetFirstPlayerController()->ClientTravel(ipText, TRAVEL_Absolute);
}
void UServerBrowserLogic::HostGame()
{
	world->GetFirstPlayerController()->ClientTravel(TEXT("Office?Listen"), TRAVEL_Absolute);
}