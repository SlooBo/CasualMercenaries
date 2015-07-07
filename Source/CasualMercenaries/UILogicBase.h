// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "UILogicBase.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API UUILogicBase : public UObject
{
	GENERATED_BODY()
	public:
		UUILogicBase();
		~UUILogicBase();
		virtual void Update(){}
private:
	
	
};
