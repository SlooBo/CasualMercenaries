// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Mine.h"
#include "MineAIController.h"


// Sets default values
AMine::AMine(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/PlayerCharacters/PlayerCharacter_Ver2/MESH_PlayerCharacter.MESH_PlayerCharacter'"));
	Mesh->SetSkeletalMesh(MeshObj.Object);
	Mesh->SetRelativeScale3D(FVector(.2,.2,.2));
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MateriaObj(TEXT("MaterialInstanceConstant'/Game/Game/PlayerCharacters/PlayerCharacter_Ver2/MAT_Playercharacter_Inst.MAT_Playercharacter_Inst'"));
	Mesh->SetMaterial(0, MateriaObj.Object);
	const ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBuleprintObj(TEXT("AnimBlueprint'/Game/Game/PlayerCharacters/PlayerCharacter_Ver2/APB_PlayerCharacter.APB_PlayerCharacter'"));
	Mesh->AnimBlueprintGeneratedClass = AnimBuleprintObj.Object->GetAnimBlueprintGeneratedClass();
	
	CharacterMovement->NavAgentProps.AgentRadius = 20;
	CharacterMovement->NavAgentProps.AgentHeight = 20;
	CharacterMovement->NavAgentProps.AgentStepHeight = 10;
	CharacterMovement->NavAgentProps.NavWalkingSearchHeightScale = 0.5;
	CharacterMovement->NavAgentProps.bCanWalk = true;
	
	CharacterMovement->MaxWalkSpeed = 400;


	AIControllerClass = AMineAIController::StaticClass();

}

// Called when the game starts or when spawned
void AMine::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMine::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AMine::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

