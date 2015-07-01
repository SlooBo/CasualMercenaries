// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/UserDefinedEnum.h"
#include "DamageTypes.generated.h"

UENUM(BlueprintType)
enum class DAMAGE_TYPE : uint8
{
	NORMAL UMETA(DisplayName = "Normal"),
	STUN UMETA(DisplayName = "Stun"),
	ROOT UMETA(DisplayName = "Root"),
};
