// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
//#include "Weap"
#include "CharacterAnimationBase.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API UCharacterAnimationBase : public UAnimInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BLueprintCallable, Category = FootPlacement)
	void UpdateCameraRotation();

	UCharacterAnimationBase(const FObjectInitializer& ObjectInitializer);
	~UCharacterAnimationBase();
	/** Left Foot Rotation, Set in Character.cpp Tick */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doge")
		float cameraRot;

	/** Left Upper Leg Offset, Set in Character.cpp Tick */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doge")
		bool wasJustShooting;
	UFUNCTION(BLueprintCallable, Category = "Doge")
	void UpdateWasJustShooting();

	UFUNCTION(BlueprintCallable, Category = "Doge")
	WEAPONID GetCurrentWeapon();
};
