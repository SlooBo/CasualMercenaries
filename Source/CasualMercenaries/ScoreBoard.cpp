// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "ScoreBoard.h"
#include "CMPlayerState.h"



UScoreBoard::UScoreBoard(const FObjectInitializer& PCIP)
{

}
UScoreBoard::~UScoreBoard()
{

}
void UScoreBoard::SetUp(UUserWidget *widget, UWorld *world)
{
	this->baseWidget = widget;
	this->world = world;

	SetValueFromWidget(&names, FString("NameBox"));
	SetValueFromWidget(&kills, FString("KillBox"));
	SetValueFromWidget(&deaths, FString("DeathBox"));
}
void UScoreBoard::Update()
{
	if (world == nullptr || !world->IsValidLowLevel())
		return;
	AGameState *gameState = world->GetGameState();
	if (gameState == nullptr || !gameState->IsValidLowLevel())
		return;
	if (names == nullptr || !names->IsValidLowLevel())
		return;
	if (kills == nullptr || !kills->IsValidLowLevel())
		return;
	if (deaths == nullptr || !deaths->IsValidLowLevel())
		return;
	names->ClearChildren();
	kills->ClearChildren();
	deaths->ClearChildren();
	for (int i = 0; i < gameState->PlayerArray.Num(); i++)
	{
		ACMPlayerState *playerState = Cast<ACMPlayerState>(gameState->PlayerArray[i]);
		bool alive = playerState->IsAlive();
		UTextBlock *nameText = NewObject<UTextBlock>();
		FString playername = FString(playerState->PlayerName);
		nameText->SetText(FText::FromString(playername));
		nameText->Justification = ETextJustify::Center;
		if (!alive)
			nameText->SetColorAndOpacity(FSlateColor(FLinearColor::Gray));
		names->AddChild(nameText);

		UTextBlock *killText = NewObject<UTextBlock>();
		FString playerkills= FString::FromInt(playerState->Score);
		killText->SetText(FText::FromString(playerkills));
		killText->Justification = ETextJustify::Center;
		if (!alive)
			killText->SetColorAndOpacity(FSlateColor(FLinearColor::Gray));
		kills->AddChild(killText);


		UTextBlock *deathText = NewObject<UTextBlock>();
		FString deathString = FString::FromInt(playerState->GetDeaths());
		deathText->SetText(FText::FromString(deathString));
		deathText->Justification = ETextJustify::Center;
		if (!alive)
			deathText->SetColorAndOpacity(FSlateColor(FLinearColor::Gray));
		deaths->AddChild(deathText);

	}
}