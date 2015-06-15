// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "NetworkSession.h"


// Sets default values
ANetworkSession::ANetworkSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ANetworkSession::OnFindSessionsComplete);
}

// Called when the game starts or when spawned
void ANetworkSession::BeginPlay()
{
	Super::BeginPlay();
	IOnlineSubsystem *test = IOnlineSubsystem::Get();
	IOnlineSessionPtr session = test->GetSessionInterface();
	FOnlineSessionSettings testSettings;
	testSettings.bAllowInvites = true;
	testSettings.bIsLANMatch = true;
	testSettings.NumPublicConnections = 8;
	testSettings.bShouldAdvertise = true;
	testSettings.bAllowJoinViaPresence = true;
	ULocalPlayer *player = nullptr;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		player = Cast<ULocalPlayer>(Iterator->Get()->Player);
	}

	auto tt = player->GetPreferredUniqueNetId().Get();
	bool succesfull = session->CreateSession(*tt, "TestSession", testSettings);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Did we create session: " + FString::FromInt(succesfull));


	 SearchSettings = MakeShareable(new FOnlineSessionSearch());
	 SearchSettings->MaxSearchResults = 8;
	 SearchSettings->bIsLanQuery = true;
	 SearchSettings->TimeoutInSeconds = 20;
	TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SearchSettings.ToSharedRef();
	OnFindSessionsCompleteDelegateHandle = session->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
	auto hosted = session->FindSessions(*tt, SearchSettingsRef);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "yay");
	//IOnlineSubSystem *test = IOnlineSubSystem::Get();
}

// Called every frame
void ANetworkSession::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ANetworkSession::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogOnlineGame, Verbose, TEXT("OnFindSessionsComplete bSuccess: %d"), bWasSuccessful);

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Results: " + FString::FromInt(SearchSettings->SearchResults.Num()));
			//UE_LOG(LogOnlineGame, Verbose, TEXT("Num Search Results: %d"), SearchSettings->SearchResults.Num());
			for (int32 SearchIdx = 0; SearchIdx < SearchSettings->SearchResults.Num(); SearchIdx++)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Test" );
				const FOnlineSessionSearchResult& SearchResult = SearchSettings->SearchResults[SearchIdx];
				DumpSession(&SearchResult.Session);
			}

			OnFindSessionsComplete().Broadcast(bWasSuccessful);
		}
	}
}