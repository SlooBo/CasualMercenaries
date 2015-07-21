// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon.h"
#include "CableComponent.h"
#include "HookProjectile.h"
#include "Hook.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class HOOKSTATE : uint8
{
		NOT_STARTED,
		PROJECTILE_SHOT,
		GOING_TOWARDS_PROJECTILE
};


UCLASS()
class CASUALMERCENARIES_API AHook : public AWeapon
{
	GENERATED_BODY()
public:

	AHook(const FObjectInitializer& FOI);
	void BeginPlay();

	// Called every frame
	void Tick(float DeltaTime);
	void PrimaryFunction(APlayerCharacter* user);
	void PrimaryFunctionReleased(APlayerCharacter* user);
	void Reload();
	void Fire();
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Cable")
		UCableComponent *cableComponent;
	void ReleaseHook();
	bool hooked;
	bool shotProjectile;
	void FlyTowardsProjectile();
	FRotator LookAtRotator(FVector lookfrom, FVector lookat);
private:
	AHookProjectile *projectile;
	FVector hookedLocation;
	FVector startLocation;
	HOOKSTATE currentState;
	FTimerHandle hookReleaseHandle;
	void MoveTick();

	float EasedValue(float currentTime, float startValue, float changeInValue, float duration);
};
