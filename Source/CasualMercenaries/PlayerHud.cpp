// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "PlayerHud.h"
#include "ServerBrowserLogic.h"
#include "ShopLogic.h"

APlayerHud::APlayerHud(const FObjectInitializer& PCIP) :Super()
{
	static ConstructorHelpers::FObjectFinder<UClass> MainMenuBP(TEXT("'/Game/Game/UI/PhoneUITest.PhoneUITest_C'"));
	if (MainMenuBP.Object){
		mainMenuClass = (UClass*)MainMenuBP.Object;
	}
	;
	static ConstructorHelpers::FObjectFinder<UClass> GameUIBP(TEXT("'/Game/Game/UI/GameUI.GameUI_C'"));
	if (GameUIBP.Object){
		gameUIClass = (UClass*)GameUIBP.Object;
	}

	static ConstructorHelpers::FObjectFinder<UClass> ServerBrowserBP(TEXT("'/Game/Game/UI/ServerBrowserGUI.ServerBrowserGUI_C'"));
	if (ServerBrowserBP.Object){
		serverBrowserClass = (UClass*)ServerBrowserBP.Object;
	}
	
		static ConstructorHelpers::FObjectFinder<UClass> ShopBP(TEXT("'/Game/Game/UI/ShopGUI.ShopGUI_C'"));
		if (ShopBP.Object){
			shopClass = (UClass*)ShopBP.Object;
	}
}
void APlayerHud::DrawHud()
{
	Super::DrawHUD();
}
void APlayerHud::BeginPlayCplusplus()
{
	//changeUIElement(MenuType::MAIN_MENU);
	//call function setup chat from chatwindow

}

// Called every frame
void APlayerHud::Tick(float DeltaSeconds)
{

}
void APlayerHud::changeUIElement(MenuType menu)
{
	for (int i = 0; i < logicClasses.Num(); i++)
	{
		logicClasses[i] = nullptr;
	}
	logicClasses.Empty();
	switch (menu)
	{
	case MenuType::MAIN_MENU:
		changeUIElement(mainMenuClass);
		//this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		break;
	case MenuType::GAME_UI:
		//this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		changeUIElement(gameUIClass);
		break;
	case MenuType::SERVER_BROWSER:
	{
		UUserWidget* widget;
		widget = changeUIElement(serverBrowserClass);
		UServerBrowserLogic* serverBrowser = NewObject<UServerBrowserLogic>();
		serverBrowser->SetUp(widget,GetWorld());
		logicClasses.Add(serverBrowser);
		break;
	}
	case MenuType::SHOP:
	{
		UUserWidget* tempWidget = changeUIElement(shopClass);
		this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		UShopLogic* shopLogic = NewObject<UShopLogic>();
		shopLogic->SetUp(tempWidget,GetWorld());
		logicClasses.Add(shopLogic);
		break;
	}
	case MenuType::NO_UI:
	default:
		break;
	}
}
UUserWidget* APlayerHud::changeUIElement(UClass *uitype)
{
	ClearAllWidgets();

	widgetInstance = CreateWidget<UUserWidget>(GetWorld(), uitype);
	widgetInstance->AddToViewport();
	widgets.Add(widgetInstance);
	return widgetInstance;
	
}
void APlayerHud::ClearAllWidgets()
{
	for (int32 i = 0; i < widgets.Num(); i++)
	{
		widgets[i]->RemoveFromViewport();
	}
	widgets.Empty();
}