// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "MudBall.h"
#include "PlayerCharacter.h"


AMudBall::AMudBall(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//StaticMesh
	projectileMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GranadeMesh"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Weapons/MudBuster/Projectile/MESH_Budbuster_projectile.MESH_Budbuster_projectile'"));
	projectileMesh->SetStaticMesh(MeshObj.Object);
	//Material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/MudBuster/Projectile/MAT_Mudbuster_projectile.MAT_Mudbuster_projectile'")); // Material missing!!!!!
	projectileMesh->SetMaterial(0, MateriaObj.Object);


	//Movement
	ProjectileMovement->ProjectileGravityScale = 0.1;
	ProjectileMovement->InitialSpeed = 2500.f;


	//CollisionComponent
	CollisionComp->InitSphereRadius(15.0f);


	//Delegate
	OnActorHit.AddDynamic(this, &AMudBall::OnMyActorHit);


	//Integers
	health = 100;
	size = 0.2;

	//Scale
	projectileMesh->SetRelativeScale3D(FVector(size, size, size));

	//Triggers
	inflating = false;


	//ParticleSystem
	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem>ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_MudSplash.P_MudSplash'"));
	flavorParticleEffect = ParticleObj.Object;


	//replication
	bReplicates = true;
	bReplicateMovement = true;
}



AMudBall::~AMudBall()
{

}

void AMudBall::OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	ProjectileMovement->SetActive(false, false);
	inflating = true;
}


void AMudBall::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!HasAuthority())
		return;
		if (health <= 0)	
			Explode();

	if (inflating)
	{		
		if (size > 1)
		{	
			inflating = false;
			return;
		}
		size += 0.1;
		projectileMesh->SetRelativeScale3D(FVector(size, size, size));
	}
}
void AMudBall::BeginPlay()
{
	Super::BeginPlay();
}

bool AMudBall::Explode_Validate()
{
	return true;
}

void AMudBall::Explode_Implementation()
{
	UParticleSystemComponent *particle = UGameplayStatics::SpawnEmitterAtLocation(this, flavorParticleEffect, this->GetActorLocation(), FRotator::ZeroRotator, true);
	particle->SetRelativeScale3D(FVector(2, 2, 2));
	Destroy();
}
