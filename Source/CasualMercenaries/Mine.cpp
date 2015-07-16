// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Mine.h"
#include "MineAIController.h"
#include "PlayerCharacter.h"


// Sets default values
AMine::AMine(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Mesh, scale, material and animation in that order
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/PlayerCharacters/PlayerCharacter_Ver2/MESH_PlayerCharacter.MESH_PlayerCharacter'"));
	GetMesh()->SetSkeletalMesh(MeshObj.Object);
	GetMesh()->SetRelativeScale3D(FVector(.2,.2,.2));
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MateriaObj(TEXT("MaterialInstanceConstant'/Game/Game/PlayerCharacters/PlayerCharacter_Ver2/MAT_Playercharacter_Inst.MAT_Playercharacter_Inst'"));
	GetMesh()->SetMaterial(0, MateriaObj.Object);
	const ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBuleprintObj(TEXT("AnimBlueprint'/Game/Game/PlayerCharacters/PlayerCharacter_Ver2/APB_PlayerCharacter.APB_PlayerCharacter'"));
	GetMesh()->AnimBlueprintGeneratedClass = AnimBuleprintObj.Object->GetAnimBlueprintGeneratedClass();
	
	//Movement
	GetCharacterMovement()->NavAgentProps.AgentRadius = 20;
	GetCharacterMovement()->NavAgentProps.AgentHeight = 20;
	GetCharacterMovement()->NavAgentProps.AgentStepHeight = 10;
	GetCharacterMovement()->NavAgentProps.NavWalkingSearchHeightScale = 0.5;
	GetCharacterMovement()->NavAgentProps.bCanWalk = true;
	
	GetCharacterMovement()->MaxWalkSpeed = 800;

	//CapsuleComponent->
	//RootComponent->
	


	//MineBehavior = 

	//Controller
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
	for (TActorIterator<APlayerCharacter> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);

		if (distance <= 100)
		{
			Explode();
		}
	}
}

// Called to bind functionality to input
void AMine::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

void AMine::Explode()
{
	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		float distance = GetDistanceTo(*It);

		if (distance <= 300)
		{
			float x = distance / 300;
			float explosionDamage = 30;
			explosionDamage*= x;

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Splosion");
			//UGameplayStatics::ApplyDamage(*aItr, explosionDamage, GetInstigatorController(), this, UDamageType::StaticClass());
			//APlayerCharacter* tempChar = Cast<APlayerCharacter>(this->GetOwner());
			//aItr->TakeDamage(explosionDamage, DAMAGE_TYPE::NORMAL, Cast<class ACMPlayerController>(tempChar->GetController()));
		}
	}
	//const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj2(TEXT("ParticleSystem'/Game/Game/Particles/P_Explosion1.P_Explosion1'"));
	//UParticleSystemComponent *particle = UGameplayStatics::SpawnEmitterAtLocation(this, ParticleObj2.Object, this->GetActorLocation(), FRotator::ZeroRotator, true);
	Destroy();
}

