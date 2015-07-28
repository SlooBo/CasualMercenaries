// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "PlayerHud.h"
#include "ServerBrowserLogic.h"
#include "ShopLogic.h"
#include "HUDLogic.h"
#include "ScoreBoard.h"
#include "Util.h"
#include "CMPlayerController.h"
#include "PlayerCharacter.h"
APlayerHud::APlayerHud(const FObjectInitializer& PCIP) :Super()
{
	currentMenu = MenuType::NO_UI;
	static ConstructorHelpers::FObjectFinder<UClass> MainMenuBP(TEXT("'/Game/Game/UI/AnimatedMainMenu.AnimatedMainMenu_C'"));
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
	for (int i = 0; i < screenMessages.Num(); i++)
	{
		if (screenMessages[i].currentTime >= screenMessages[i].maxTime)
		{
			if (screenMessages[i].widget != nullptr && screenMessages[i].widget->IsValidLowLevel())
			{
				screenMessages[i].widget->RemoveFromViewport();
				screenMessages.RemoveAt(i);
				i--;
			}
		}
		else
			screenMessages[i].currentTime += DeltaSeconds;
	}
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
	ClearAllWidgets();

	//Give player input back if closing shop
	if (GetCurrentUI() == MenuType::SHOP)
	{
		ACMPlayerController *controller = Cast<ACMPlayerController>(GetWorld()->GetFirstPlayerController());
		APlayerCharacter *player = Cast<APlayerCharacter>(controller->GetPawn());
		if (player != nullptr)
			player->RestoreActivity();
	}

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
		this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
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
		this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		UUserWidget* widget;
		widget = changeUIElement(serverBrowserClass);
		UServerBrowserLogic* serverBrowser = NewObject<UServerBrowserLogic>();
		serverBrowser->SetUp(widget,GetWorld());
		logicClasses.Add(serverBrowser);
		break;
	}
	case MenuType::SHOP:
	{
		//also show hud
		UUserWidget* widget;

		widget = changeUIElement(gameUIClass);
		UHUDLogic* hudLogic = NewObject<UHUDLogic>();
		hudLogic->SetUp(widget, GetWorld());
		logicClasses.Add(hudLogic);

		UUserWidget* tempWidget = changeUIElement(shopClass);
		this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		UShopLogic* shopLogic = NewObject<UShopLogic>();
		shopLogic->SetUp(tempWidget, GetWorld());
		logicClasses.Add(shopLogic);
		break;
	}
	case MenuType::PAUSE_MENU:
	{
		this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		changeUIElement(pauseClass);
		break;
	}
	case MenuType::SCOREBOARD:
	{
		this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		UClass *scoreboardClass = Util::LoadObjFromPath<UClass>(TEXT("'/Game/Game/UI/ScoreBoard.ScoreBoard_C'"));
		UUserWidget *widget = changeUIElement(scoreboardClass);
		UScoreBoard *scoreboard = NewObject<UScoreBoard>();
		scoreboard->SetUp(widget, GetWorld());
		logicClasses.Add(scoreboard);
		break;
	}
	case MenuType::NO_UI:
	default:
		break;
	}
}
UUserWidget* APlayerHud::changeUIElement(UClass *uitype)
{


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
UUILogicBase *APlayerHud::GetCurrentUILogic()
{
	if (logicClasses.IsValidIndex(0))
	{
		return logicClasses[0];
	}
	return nullptr;
}
void APlayerHud::CreateTestHavoc()
{
		UUserWidget *havoc = CreateWidget<UUserWidget>(GetWorld(), Util::LoadObjFromPath<UClass>(TEXT("'/Game/Game/UI/Wreak_Havoc_Final.Wreak_Havoc_Final_C'")));
		havoc->AddToViewport(1);
		UBlueprint *bp = Cast<UBlueprint>(havoc);
	return;
}
void APlayerHud::ShowText(FString text, int32 fontsize, float anchorX, float anchorY , int32 lifetime, FLinearColor color)
{
	UClass *widgetType = Util::LoadObjFromPath<UClass>(TEXT("'/Game/Game/UI/OnScreenMessage.OnScreenMessage_C'"));
	UUserWidget *widgetInstance = CreateWidget<UUserWidget>(GetWorld(), widgetType);

	TArray<UWidget*> children;
	widgetInstance->WidgetTree->GetAllWidgets(children);

	int childcount = children.Num();

	UTextBlock *textblock = Cast<UTextBlock>(children[1]);
	textblock->RenderTransform.Translation = FVector2D(0, 0);
	textblock->SetText(FText::FromString(text));
	textblock->Font.Size = fontsize;
	textblock->ColorAndOpacity = FSlateColor(color);
	UCanvasPanelSlot *panelslot = Cast<UCanvasPanelSlot>(textblock->Slot);

	//Anchors (between 0.0-1.0)
	panelslot->LayoutData.Anchors.Maximum.X = anchorX;
	panelslot->LayoutData.Anchors.Minimum.X = anchorX;

	panelslot->LayoutData.Anchors.Maximum.Y = anchorY;
	panelslot->LayoutData.Anchors.Minimum.Y = anchorY;

	widgetInstance->AddToViewport(1);
	FScreenMessage message;
	message.widget = widgetInstance;
	message.maxTime = lifetime;
	screenMessages.Add(message);
	//widgetInstance->Slot;
}