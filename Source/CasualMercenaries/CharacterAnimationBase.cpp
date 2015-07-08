// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CharacterAnimationBase.h"

#include "PlayerCharacter.h"

void UCharacterAnimationBase::UpdateCameraRotation()
{
	APlayerCharacter *player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (player == nullptr)
		cameraRot = 0.0f;
	cameraRot =  player->GetCamera()->GetComponentRotation().Pitch;
}
UCharacterAnimationBase::UCharacterAnimationBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}
UCharacterAnimationBase::~UCharacterAnimationBase()
{

}