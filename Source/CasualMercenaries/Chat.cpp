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
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Child count: " +FString::FromInt(children.Num()));
	
	
	int childcount = children.Num();
	for (int i = 0; i < childcount; i++)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Child name: " + children[i]->GetName() +","+FString::FromInt(i));
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
	for (int i = 0; i < 1; i++)
		AddText("test" + i);

}
void UChat::AddText(FString text)
{
	UUserWidget *widgetInstance = CreateWidget<UUserWidget>(world, chatText);
	if (textBox->GetChildrenCount() == 0)
	textBox->AddChild(widgetInstance);
	else
	textBox->InsertChildAt(textBox->GetChildrenCount()-1, widgetInstance);
	//widgetInstance->RemoveFromParent();
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "COUNT::" + FString::FromInt(textBox->GetChildIndex(widgetInstance)));
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
	switch (CommitMethod)
	{
	case ETextCommit::OnEnter:
		AddText(textInput->GetText().ToString());
		ChangePlayerInputVisibility();
		break;
	case ETextCommit::OnUserMovedFocus:
		ChangePlayerInputVisibility();
		break;
	default:
		break;
	}
}
void UChat::SetInputModeGameOnly()
{
	APlayerCharacter *localPlayer = Util::GetLocalPlayer(world);
	APlayerController *controller = Cast<APlayerController>(localPlayer->Controller);
	if (controller != nullptr)
	{
		FInputModeGameOnly InputMode;
		controller->SetInputMode(InputMode);
	}
}