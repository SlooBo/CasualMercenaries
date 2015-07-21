// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "HookProjectile.h"





AHookProjectile::AHookProjectile(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	//StaticMesh
	projectileMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RocketMesh"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Weapons/Hook/hook.hook'"));
	projectileMesh->SetStaticMesh(MeshObj.Object);
	projectileMesh->IgnoreActorWhenMoving(this->GetOwner(), true);

	//RootComponent = projectileMesh;


	CollisionComp->IgnoreActorWhenMoving(this->GetOwner(), true);
	//Material MISSING!

	//Movement
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0;

	//Delegate
	OnActorHit.AddDynamic(this, &AHookProjectile::OnMyActorHit);



	//Stuff
	SetActorEnableCollision(true);


	//Integers
	lifeTime = 10;





	//Replication
	bReplicates = true;
	bReplicateMovement = true;

	hit = false;


	//audioComp->SetSound(audioList[0]);
	//audioComp->Play();
}

AHookProjectile::~AHookProjectile()
{

}

void AHookProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	projectileMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	if (!HasAuthority())
		return;
	livedTime += DeltaSeconds;
	if (livedTime >= lifeTime)
	{
			Destroy();
	}
}



void AHookProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AHookProjectile::OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	APlayerCharacter* temp = Cast<APlayerCharacter>(OtherActor);
	if (temp == nullptr)
	{
		ProjectileMovement->Velocity = FVector::ZeroVector;
		hit = true;
	}
}

