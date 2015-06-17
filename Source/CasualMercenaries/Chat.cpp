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
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Child count: " +FString::FromInt(children.Num()));
	textInput = Cast<UEditableTextBox>(children[11]);
	int childcount = children.Num();
	for (int i = 0; i < childcount; i++)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Child name: " + children[i]->GetName() +","+FString::FromInt(i));
		UScrollBox *scrollbox = Cast<UScrollBox>(children[i]);
		if (scrollbox != nullptr)
		{
			textBox = scrollbox;
			break;
		}
	}
	for (int i = 0; i < 1; i++)
		AddText("test" + i);

}
void UChat::AddText(FString text)
{
	UUserWidget *widgetInstance = CreateWidget<UUserWidget>(world, chatText);
	textBox->AddChild(widgetInstance);

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
	isInputVisible = true;
	SetInputModeGameAndUI();
	textInput->SetText(FText::FromString(text));
}