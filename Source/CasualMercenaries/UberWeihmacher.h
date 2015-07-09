// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Weapon.h"
#include "UberWeihmacher.generated.h"

UCLASS()
class CASUALMERCENARIES_API AUberWeihmacher : public AWeapon
{
	GENERATED_BODY()
	
public:	

	/************************************************************************/
	/* Defaults                                                             */
	/************************************************************************/

	// Sets default values for this actor's properties
	AUberWeihmacher(const FObjectInitializer& FOI);

	// Called when the game starts or when spawned
	void BeginPlay();

	// Called every frame
	void Tick( float DeltaSeconds );

	/************************************************************************/
	/* Functionality                                                        */
	/************************************************************************/

	void PrimaryFunction(APlayerCharacter* user);

	void PrimaryFunctionReleased(APlayerCharacter* user);

	void SecondaryFunction(APlayerCharacter* user);

	void Reload()
	{
		ammo += 10;
		if (ammo >= 100)
			ammo = 100;
	};


private:

	/************************************************************************/
	/* Hidden functionality                                                 */
	/************************************************************************/

	void Fire();
	
	/************************************************************************/
	/* Variables                                                            */
	/************************************************************************/

	float burntime;
	float pressure;
};
