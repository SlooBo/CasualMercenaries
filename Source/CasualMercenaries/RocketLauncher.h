// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "Projectile.h"
#include "RocketLauncher.generated.h"

UCLASS()
class CASUALMERCENARIES_API ARocketLauncher : public AWeapon
{
	GENERATED_BODY()

public:



	ARocketLauncher(const FObjectInitializer& FOI);

	void PrimaryFunction(APlayerCharacter* user);

	void SecondaryFunction();

	void BeginPlay();

	// Called every frame
	void Tick(float DeltaSeconds);
	void Reload() {};


};
