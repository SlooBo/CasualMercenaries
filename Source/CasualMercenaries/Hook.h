// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon.h"
#include "MyCable.h"
#include "Hook.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API AHook : public AWeapon
{
	GENERATED_BODY()
public:

	AHook(const FObjectInitializer& FOI);
	void BeginPlay();

	// Called every frame
	void Tick(float DeltaTime);
	void AHook::PrimaryFunction(APlayerCharacter* user);
	void PrimaryFunctionReleased(APlayerCharacter* user);
	void Reload();
	void Fire();
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cable")
		UMyCable *cableComponent;
	void ReleaseHook();
	bool hooked;
private:
	FVector hookedLocation;
	FVector startLocation;

	FTimerHandle hookReleaseHandle;


	float EasedValue(float currentTime, float startValue, float changeInValue, float duration);
};
