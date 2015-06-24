// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "ServerBrowserLogic.h"
#include "NetworkSession.h"



UServerBrowserLogic::UServerBrowserLogic() :Super()
{
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
	bool succesfull = session->CreateSession(tt, "TestSession", testSettings);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Did we create session: " + FString::FromInt(succesfull));
}