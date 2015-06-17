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
	for (int i = 0; i < children.Num(); i++)
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
	UWidget *widgetInstance = CreateWidget<UUserWidget>(world, chatText);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Child count: " + FString::FromInt(textBox->GetChildrenCount()));
	textBox->AddChild(widgetInstance);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Child count: " + FString::FromInt(textBox->GetChildrenCount()));

}

void UChat::OpenAllChat()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Allchat!");
	textBox->SetVisibility(ESlateVisibility::Visible);
	world->GetFirstPlayerController()->bShowMouseCursor = true;
	isInputVisible = true;
	SetInputModeGameAndUI();
	//TODO merge these
}
void UChat::OpenTeamChat()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "TeamChat!");
	textBox->SetVisibility(ESlateVisibility::Visible);
	world->GetFirstPlayerController()->bShowMouseCursor = true;
	isInputVisible = true;
	SetInputModeGameAndUI();
	//TODO merge these
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