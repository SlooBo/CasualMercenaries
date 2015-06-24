// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "PlayerHud.generated.h"

/**
 * 
 */

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class MenuType : uint8
{
	MAIN_MENU 	UMETA(DisplayName = "MAIN_MENU"),
	GAME_UI 	UMETA(DisplayName = "GAME_UI"),
	NO_UI	UMETA(DisplayName = "NO_UI")
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


private:
	UClass* chatwindowClass;
	UPROPERTY()
	TArray<UUserWidget*> widgets;
	void changeUIElement(UClass *uitype);
	UPROPERTY()
	UClass *mainMenuClass;
	UPROPERTY()
	UClass *gameUIClass;
	void ClearAllWidgets();
	UUserWidget* widgetInstance;
	
};
