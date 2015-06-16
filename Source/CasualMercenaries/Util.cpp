// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Util.h"
#include "PlayerCharacter.h"
APlayerCharacter *Util::GetLocalPlayer()
{
	return Cast<APlayerCharacter>(GEngine->GetWorld()->GetFirstPlayerController());
}