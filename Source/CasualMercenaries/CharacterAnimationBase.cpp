// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CharacterAnimationBase.h"

#include "PlayerCharacter.h"
#include "CMPlayerController.h"

void UCharacterAnimationBase::UpdateCameraRotation()
{
	if (GetWorld() == nullptr)
		return;
	APlayerCharacter *player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (player == nullptr)
	{
		cameraRot = 0.0f;
		return;
	}
	cameraRot =  player->GetCamera()->GetComponentRotation().Pitch;
}
UCharacterAnimationBase::UCharacterAnimationBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	wasJustShooting = false;
}
UCharacterAnimationBase::~UCharacterAnimationBase()
{

}
void UCharacterAnimationBase::UpdateWasJustShooting()
{
	if (GetWorld() == nullptr)
		return;
	ACMPlayerController *controller = Cast<ACMPlayerController>(GetWorld()->GetFirstPlayerController());
	if (controller == nullptr)
		return;
	AWeapon *weapon = controller->GetInventory().GetWeapon(controller->GetInventory().currentWeapon);
	wasJustShooting = weapon->GetJustFired();
}