// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "ScoreBoard.h"




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
	SetValueFromWidget(&scores, FString("ScoreBox"));
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
	if (scores == nullptr || !scores->IsValidLowLevel())
		return;
	names->ClearChildren();
	scores->ClearChildren();
	for (int i = 0; i < gameState->PlayerArray.Num(); i++)
	{
		UTextBlock *nameText = NewObject<UTextBlock>();
		FString playername = FString(gameState->PlayerArray[i]->PlayerName);
		nameText->SetText(FText::FromString(playername));
		names->AddChild(nameText);

		UTextBlock *scoreText = NewObject<UTextBlock>();
		FString playerscore = FString::FromInt(gameState->PlayerArray[i]->Score);
		scoreText->SetText(FText::FromString(playerscore));
		scores->AddChild(scoreText);
	}
}