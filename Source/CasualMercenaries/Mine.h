// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Mine.generated.h"


UCLASS()
class CASUALMERCENARIES_API AMine : public ACharacter
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	AMine(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree* MineBehavior;

	void Explode();

};
