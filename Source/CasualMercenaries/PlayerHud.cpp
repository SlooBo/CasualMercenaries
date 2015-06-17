// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "PlayerHud.h"
//#include "Util.h"
#include "Chat.h"



APlayerHud::APlayerHud(const class FPostConstructInitializeProperties& PCIP) :Super()
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> MainMenuBP(TEXT("WidgetBlueprint'/Game/Game/UI/PhoneUITest.PhoneUITest'"));
	if (MainMenuBP.Object){
		mainMenuClass = (UClass*)MainMenuBP.Object->GeneratedClass;
	}
	;
	static ConstructorHelpers::FObjectFinder<UBlueprint> GameUIBP(TEXT("WidgetBlueprint'/Game/Game/UI/GameUI.GameUI'"));
	if (GameUIBP.Object){
		gameUIClass = (UClass*)GameUIBP.Object->GeneratedClass;
	}
	
	static ConstructorHelpers::FObjectFinder<UBlueprint> ChatTextBP(TEXT("WidgetBlueprint'/Game/Game/UI/TextWidget.TextWidget'"));
	if (ChatTextBP.Object){
		UClass *chatText = (UClass*)ChatTextBP.Object->GeneratedClass;
		chat = new Chat(chatText,this->GetWorld());
	}

	
}
void APlayerHud::DrawHud()
{
	Super::DrawHUD();
}
void APlayerHud::BeginPlay()
{
	changeUIElement(MenuType::MAIN_MENU);


}

// Called every frame
void APlayerHud::Tick(float DeltaSeconds)
{

}
void APlayerHud::changeUIElement(MenuType menu)
{
	switch (menu)
	{
	case MenuType::MAIN_MENU:
		changeUIElement(mainMenuClass);
		break;
	case MenuType::GAME_UI:
		changeUIElement(gameUIClass);
		SetUpChat();
		break;
	case MenuType::NO_UI:
	default:
		break;
	}
}
void APlayerHud::changeUIElement(UClass *uitype)
{
	ClearAllWidgets();

	widgetInstance = CreateWidget<UUserWidget>(GetWorld(), uitype);
	widgetInstance->AddToViewport();
	this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	widgets.Add(widgetInstance);
}
void APlayerHud::ClearAllWidgets()
{
	for (int32 i = 0; i < widgets.Num(); i++)
	{
		widgets[i]->RemoveFromViewport();
	}
	widgets.Empty();
}
void APlayerHud::SetUpChat()
{
	chat->Initialize(widgetInstance);
}
void APlayerHud::AddText(FString text)
{
	chat->AddText(text);
}
Chat *APlayerHud::GetChat()
{
	return chat;
}