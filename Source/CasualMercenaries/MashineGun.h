// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Weapon.h"
#include "MashineGun.generated.h"
/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API AMashineGun : public AWeapon
{

	GENERATED_BODY()

public:

	AMashineGun(const FObjectInitializer& FOI);
	~AMashineGun();

};
