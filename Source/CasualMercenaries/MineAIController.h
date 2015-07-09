// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "MineAIController.generated.h"


/**
 * 
 */
//class APawn;

UCLASS()
class CASUALMERCENARIES_API AMineAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	UPROPERTY(transient)
	UBlackboardComponent* blackboardComponent;


	UPROPERTY(transient)
	UBehaviorTreeComponent* behaviorComponent;

	virtual void Possess(class APawn *InPawn) override;
	//
	void SetEnemy(class APawn *InPawn);

	UFUNCTION(BlueprintCallable, Category = Behavior)
		void SearchForEnemy();

	virtual void Tick(float DeltaTime);

	AMineAIController(const class FPostConstructInitializeProperties& PCIP);



private:
	
	uint8 enemyKeyID;
	uint8 enemyLocationID;

};
