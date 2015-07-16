// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "MineAIController.h"
#include "Mine.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"//
#include "BehaviorTree/BlackboardComponent.h"//
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "PlayerCharacter.h"


AMineAIController::AMineAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	blackboardComponent = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));

	behaviorComponent = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
}

void AMineAIController::Possess(class APawn* InPawn)
{
	Super::Possess(InPawn);
	AMine* Mine = Cast<AMine>(InPawn);
	if (Mine && Mine->MineBehavior)
	{
		blackboardComponent->InitializeBlackboard(*Mine->MineBehavior->BlackboardAsset);

		enemyKeyID = blackboardComponent->GetKeyID("TargetToFollow");
		enemyLocationID = blackboardComponent->GetKeyID("VectorKey");

		behaviorComponent->StartTree(*(Mine->MineBehavior));
	}
}

void AMineAIController::Tick(float DeltaTime)
{


}

void AMineAIController::SearchForEnemy()
{
	APawn* mine = GetPawn();
	if (mine == NULL)
		return;


	const FVector Loc = mine->GetActorLocation();
	float bestDistSq = MAX_FLT;
	APlayerCharacter* bestTarget = NULL;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{
		APlayerCharacter* testTarget = Cast<APlayerCharacter>(*It);
		if (testTarget)
		{
			(testTarget->GetActorLocation() - Loc).SizeSquared();
			const float distSQ = (testTarget->GetActorLocation() - Loc).SizeSquared();
			if (distSQ < bestDistSq)
			{
				bestDistSq = distSQ;
				bestTarget = testTarget;
			}

		}
	}
	if (bestTarget)
	{
		SetEnemy(bestTarget);
	}
}

void AMineAIController::SetEnemy(class APawn* InPawn)
{
	blackboardComponent->SetValue<UBlackboardKeyType_Object>(enemyKeyID, InPawn);
	blackboardComponent->SetValue<UBlackboardKeyType_Vector>(enemyLocationID, InPawn->GetActorLocation());
}
