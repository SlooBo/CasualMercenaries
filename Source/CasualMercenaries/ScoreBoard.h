// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UILogicBase.h"
#include "ScoreBoard.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API UScoreBoard : public UUILogicBase
{
	GENERATED_BODY()
public:
		UScoreBoard(const FObjectInitializer& PCIP);
	~UScoreBoard();
	void SetUp(UUserWidget *widget, UWorld *world);
	void Update() override;
private:
	UWorld *world;
	UScrollBox *names;
	UScrollBox *scores;
	UScrollBox *kills;
	UScrollBox *deaths;

};
