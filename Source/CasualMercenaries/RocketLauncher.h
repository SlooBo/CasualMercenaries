// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Weapon.h"
#include "Projectile.h"
/**
 * 
 */
class CASUALMERCENARIES_API RocketLauncher : public AWeapon
{

public:

	RocketLauncher(const FObjectInitializer& FOI);
	~RocketLauncher();

	void PrimaryFunction(AActor* user);

	void SecondaryFunction();

};
