// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "UILogicBase.h"
#include "PlayerHud.generated.h"

/**
 * 
 */

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class MenuType : uint8
{
	MAIN_MENU 		UMETA(DisplayName = "MAIN_MENU"),
	GAME_UI 		UMETA(DisplayName = "GAME_UI"),
	SERVER_BROWSER 	UMETA(DisplayName = "SERVER_BROWSER"),
	SHOP			UMETA(DisplayName = "SHOP"),
	NO_UI			UMETA(DisplayName = "NO_UI")
};

UCLASS()
class CASUALMERCENARIES_API APlayerHud : public AHUD
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlayerHud(const FObjectInitializer& PCIP);
	// Called when the game starts or when spawned
	//had to be changed so blueprint can run
	UFUNCTION(BlueprintCallable, Category = "UI")
		void BeginPlayCplusplus();
	//virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	void DrawHud();
	UFUNCTION(BlueprintCallable, Category = "UI")
	void changeUIElement(MenuType menu);
	MenuType GetCurrentUI();

private:
	UPROPERTY()
	MenuType currentMenu;
	UPROPERTY()
	TArray<UUserWidget*> widgets;
	UUserWidget* changeUIElement(UClass *uitype);

	UPROPERTY()
	UClass* chatwindowClass;
	UPROPERTY()
	UClass *mainMenuClass;
	UPROPERTY()
	UClass *gameUIClass;
	UPROPERTY()
	UClass *serverBrowserClass;
	UPROPERTY()
	UClass *shopClass;
	UPROPERTY()
	TArray<UUILogicBase*> logicClasses;
	void ClearAllWidgets();
	UUserWidget* widgetInstance;
	
};
