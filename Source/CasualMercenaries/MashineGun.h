// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/Actor.h"
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

	void PrimaryFunction(APlayerCharacter* user);

	void PrimaryFunctionReleased(APlayerCharacter* user);

	void SecondaryFunction(APlayerCharacter* user);

	void BeginPlay();

	void Reload();


	// Called every frame
	void Tick(float DeltaSeconds);




private:

	void Fire();

	void DrawLine(FVector begin, FVector end);

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ServerDrawLine(FVector begin, FVector end);


};
