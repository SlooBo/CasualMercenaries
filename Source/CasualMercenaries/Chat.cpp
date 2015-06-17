// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Chat.h"

Chat::Chat(UClass *chatText, UWorld *world)
{
	isBeingUpdated = false;
	this->chatText = chatText;
	this->world = world;
}

Chat::~Chat()
{
}
void Chat::Initialize(UUserWidget *chatWidget)
{
	this->chatWidget = chatWidget;

	UWidgetTree *widgetTree = chatWidget->WidgetTree;
	TArray<UWidget*> children;
	widgetTree->GetAllWidgets(children);
	for (int i = 0; i < children.Num(); i++)
	{
		UScrollBox *scrollbox = Cast<UScrollBox>(children[i]);
		if (scrollbox != nullptr)
		{
			textBox = scrollbox;
			break;
		}
	}
	for (int i = 0; i < 1; i++)
		AddText("test" + i);
	isBeingUpdated = true;
	
}
void Chat::AddText(FString text)
{
		UWidget *widgetInstance = CreateWidget<UUserWidget>(world, chatText);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Child count: " + FString::FromInt(textBox->GetChildrenCount()));
		textBox->AddChild(widgetInstance);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Child count: " + FString::FromInt(textBox->GetChildrenCount()));
	
}

void Chat::OpenAllChat()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Allchat!" );
	textBox->SetVisibility(ESlateVisibility::Visible);
	world->GetFirstPlayerController()->bShowMouseCursor = true;
}
void Chat::OpenTeamChat()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "TeamChat!");
	textBox->SetVisibility(ESlateVisibility::Visible);
	world->GetFirstPlayerController()->bShowMouseCursor = true;
}