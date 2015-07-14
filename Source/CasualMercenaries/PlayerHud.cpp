// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "PlayerHud.h"
#include "ServerBrowserLogic.h"
#include "ShopLogic.h"
#include "HUDLogic.h"
APlayerHud::APlayerHud(const FObjectInitializer& PCIP) :Super()
{
	currentMenu = MenuType::NO_UI;
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
	static ConstructorHelpers::FObjectFinder<UClass> PauseBP(TEXT("'/Game/Game/UI/PauseMenu.PauseMenu_C'"));
	if (PauseBP.Object){
		pauseClass = (UClass*)PauseBP.Object;
	}
}
void APlayerHud::DrawHUD()
{
	Super::DrawHUD();
	for (int i = 0; i < logicClasses.Num(); i++)
	{
		if (logicClasses[i]->IsValidLowLevel())
		{
			logicClasses[i]->Update();
		}
	}
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
	currentMenu = menu;
	for (int i = 0; i < logicClasses.Num(); i++)
	{
		logicClasses[i]->ConditionalBeginDestroy();
		logicClasses[i] = NULL;
	}
	logicClasses.Empty();
	this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	switch (menu)
	{
	case MenuType::MAIN_MENU:
	{
		changeUIElement(mainMenuClass);
		//this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		break;
	}
	case MenuType::GAME_UI:
	{
		//this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		UUserWidget* widget;

		widget = changeUIElement(gameUIClass);
		UHUDLogic* hudLogic = NewObject<UHUDLogic>();
		hudLogic->SetUp(widget, GetWorld());
		logicClasses.Add(hudLogic);
		break;
	}
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
	case MenuType::PAUSE_MENU:
	{
		changeUIElement(pauseClass);
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
MenuType APlayerHud::GetCurrentUI()
{
	return currentMenu;
}
void APlayerHud::ChangeUI(int32 interfaceID)
{
	changeUIElement(static_cast<MenuType>(interfaceID));
}