// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "PlayerHud.h"
//#include "Util.h"




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
		break;
	case MenuType::NO_UI:
	default:
		break;
	}
}
void APlayerHud::changeUIElement(UClass *uitype)
{
	for (int32 i = 0; i < widgets.Num(); i++)
	{
		widgets[i]->RemoveFromViewport();
	}
	widgets.Empty();
	UUserWidget* widgetInstance;
	widgetInstance = CreateWidget<UUserWidget>(GetWorld(), uitype);
	widgetInstance->AddToViewport();
	this->GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	widgets.Add(widgetInstance);
}