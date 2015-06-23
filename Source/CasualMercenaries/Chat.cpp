// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Chat.h"
#include "Util.h"
#include "PlayerCharacter.h"
UChat::UChat(const FObjectInitializer& PCIP) :Super()
{
	this->chatText = chatText;
	isInputVisible = true;

	static ConstructorHelpers::FObjectFinder<UBlueprint> ChatTextBP(TEXT("WidgetBlueprint'/Game/Game/UI/TextWidget.TextWidget'"));
	if (ChatTextBP.Object){
		chatText = (UClass*)ChatTextBP.Object->GeneratedClass;
		
	}
}

UChat::~UChat()
{
}
void UChat::Initialize(UUserWidget *chatWidget,UWorld *world)
{
	this->world = world;
	this->chatWidget = chatWidget;

	UWidgetTree *widgetTree = chatWidget->WidgetTree;
	TArray<UWidget*> children;
	widgetTree->GetAllWidgets(children);
	
	
	int childcount = children.Num();
	for (int i = 0; i < childcount; i++)
	{
		UScrollBox *scrollbox = Cast<UScrollBox>(children[i]);
		if (scrollbox != nullptr)
		{
			textBox = scrollbox;
			continue;
		}
		UEditableTextBox *tempTextBox= Cast<UEditableTextBox>(children[i]);
		if (tempTextBox != nullptr)
			textInput = tempTextBox;

	}
	textInput->OnTextCommitted.AddDynamic(this, &UChat::OnTextInputTextCommitted);
	ChangePlayerInputVisibility();
	//chatData = Util::GetChatData(world);
}
void UChat::AddText(FString text)
{
	UUserWidget *widgetInstance = CreateWidget<UUserWidget>(world, chatText);
	chatWidgets.Insert(widgetInstance, 0);
	UpdateTextBox();
	UWidgetTree *widgetTree = widgetInstance->WidgetTree;
	TArray<UWidget*> children;
	widgetTree->GetAllWidgets(children);
	UWidget *last = children[1];
	UTextBlock *textblock = Cast<UTextBlock>(last);
	textblock->SetText(FText::FromString(text));
}

void UChat::OpenAllChat()
{
	OpenChatWithText("/all ");
}
void UChat::OpenTeamChat()
{
	OpenChatWithText("/team ");
}
void UChat::ChangePlayerInputVisibility()
{
	isInputVisible = !isInputVisible;
	if (!isInputVisible)
	{
		textInput->SetText(FText::FromString(""));
		textInput->SetVisibility(ESlateVisibility::Hidden);
		SetInputModeGameOnly();
		world->GetFirstPlayerController()->bShowMouseCursor = false;
	}
	else
	{
		textInput->SetVisibility(ESlateVisibility::Visible);
		SetInputModeGameAndUI();
		world->GetFirstPlayerController()->bShowMouseCursor = true;
	}
}
bool UChat::GetIsInputVisible()
{
	return isInputVisible;
}

void UChat::SetIsInputVisible(bool value)
{
	isInputVisible = value;
}
void UChat::SetInputModeGameAndUI()
{
	
	APlayerCharacter *localPlayer = Util::GetLocalPlayer(world);
	APlayerController *controller = Cast<APlayerController>(localPlayer->Controller);
	if (controller != nullptr)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewport(false); //TODO change?
		InputMode.SetHideCursorDuringCapture(true); //TODO change?

		if (chatWidget != nullptr)
		{
			InputMode.SetWidgetToFocus(textInput->TakeWidget());
		}
		controller->SetInputMode(InputMode);
	}
}
void UChat::OpenChatWithText(FString text)
{
	textBox->SetVisibility(ESlateVisibility::Visible);
	world->GetFirstPlayerController()->bShowMouseCursor = true;
	SetInputModeGameAndUI();
	textInput->SetText(FText::FromString(text));
	ChangePlayerInputVisibility();
}
void UChat::OnTextInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	//Is being called when player does something like move out of focus or presses enter to send text
	switch (CommitMethod)
	{
	case ETextCommit::OnEnter:
		//Util::GetChatData(world)->ServerAddText(textInput->GetText().ToString());
		static_cast<APlayerCharacter*>(world->GetFirstPlayerController()->GetPawn())->ServerAddChat(textInput->GetText().ToString());
		//AddText(textInput->GetText().ToString());
		//UpdateTextBox();
		ChangePlayerInputVisibility();
		break;
	case ETextCommit::OnUserMovedFocus:
		//hides player input box
		ChangePlayerInputVisibility();
		break;
	default:
		break;
	}
}
void UChat::SetInputModeGameOnly()
{
	//Makes player's focus into game from chatbox
	APlayerCharacter *localPlayer = Util::GetLocalPlayer(world);
	APlayerController *controller = Cast<APlayerController>(localPlayer->Controller);
	if (controller != nullptr)
	{
		FInputModeGameOnly InputMode;
		controller->SetInputMode(InputMode);
	}
}
void UChat::UpdateTextBox()
{
	//First clears chatbox from all messages
	for (int i = 0; i < chatWidgets.Num(); i++)
	{
		chatWidgets[i]->RemoveFromParent();
	}
	//then adds all messages to chatbox again, this time in right order.
	for (int i = 0; i < chatWidgets.Num(); i++)
	{
		textBox->AddChild(chatWidgets[i]);
	}

}