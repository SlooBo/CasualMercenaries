// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "MudBall.h"


UPROPERTY(VisibleAnywhere, Category = Particles)
UParticleSystemComponent* particleSystem;



AMudBall::AMudBall(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	health = 100;
	inflating = false;
	size = 0.2;

	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GranadeMesh"));
	//Mesh->AttachParent = CollisionComp;

	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Weapons/MudBuster/Projectile/MESH_Budbuster_projectile.MESH_Budbuster_projectile'"));
	Mesh->SetStaticMesh(MeshObj.Object);

	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/MudBuster/Projectile/MAT_Mudbuster_projectile.MAT_Mudbuster_projectile'")); // Material missing!!!!!
	Mesh->SetMaterial(0, MateriaObj.Object);




	Mesh->SetRelativeScale3D(FVector(size, size, size));
	//Mesh->SetSimulatePhysics(true);


	ProjectileMovement->ProjectileGravityScale = 0.1;
	ProjectileMovement->InitialSpeed = 1600.f;


	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_Explosion1.P_Explosion1'"));

	part = ParticleObj.Object;


	bReplicates = true;
	bReplicateMovement = true;




	CollisionComp->InitSphereRadius(15.0f);
	OnActorHit.AddDynamic(this, &AMudBall::OnMyActorHit);


	//const ConstructorHelpers::FObjectFinder< UAnimBlueprint > anim (TEXT("AnimSequence'/Game/Game/Weapons/MudBuster/Mudball/ANIM_Mudball_Enlarge.ANIM_Mudball_Enlarge'"));
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
		Mesh->SetRelativeScale3D(FVector(size, size, size));
		
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
	Destroy();
}
