// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CharacterAnimationBase.h"

#include "PlayerCharacter.h"
#include "CMPlayerController.h"

void UCharacterAnimationBase::UpdateCameraRotation()
{
	if (GetWorld() == nullptr)
		return;
	ACMPlayerController *controller = Cast<ACMPlayerController>(GetWorld()->GetFirstPlayerController());
	if (controller == nullptr)
		return;
	 APlayerCharacter *player = Cast<APlayerCharacter>(controller->GetPawn());
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
	if (weapon != nullptr)
		wasJustShooting = weapon->GetJustFired() && weapon->GetAmmo() != 0;
	else
		wasJustShooting = false;
	if (wasJustShooting)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "WasJustSHooting = true");

}