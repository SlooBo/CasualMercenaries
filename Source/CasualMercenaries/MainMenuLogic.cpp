// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "MainMenuLogic.h"

UMainMenuLogic::UMainMenuLogic()
{

}
UMainMenuLogic::~UMainMenuLogic()
{

}

void UMainMenuLogic::SetUp(UUserWidget *widget, UWorld *world)
{
	this->baseWidget = widget;
	this->world = world;
	
	SetValueFromWidget(&versionText, "VersionText");

	if (versionText != NULL)
		versionText->SetText(FText::FromString("Assembly Build " + GetProjectVersion()));
}

FString UMainMenuLogic::GetProjectVersion()
{
	FString version;
	GConfig->GetString(TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectVersion"), version, GGameIni);
	return version;
}