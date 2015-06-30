// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "MudBall.h"


UPROPERTY(VisibleAnywhere, Category = Particles)
UParticleSystemComponent* particleSystem;



AMudBall::AMudBall(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GranadeMesh"));

	this->RootComponent = Mesh;

	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Weapons/MudBuster/Projectile/MESH_Budbuster_projectile.MESH_Budbuster_projectile'")); // MESH missing!!!!!
	Mesh->SetStaticMesh(MeshObj.Object);

	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/MudBuster/Projectile/MAT_Mudbuster_projectile.MAT_Mudbuster_projectile'")); // Material missing!!!!!
	Mesh->SetMaterial(0, MateriaObj.Object);

	Mesh->SetRelativeScale3D(FVector(.2, .2, .2));
	Mesh->SetSimulatePhysics(true);


	ProjectileMovement->ProjectileGravityScale = 0.1;
	ProjectileMovement->InitialSpeed = 1000.f;


	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_Explosion1.P_Explosion1'"));

	part = ParticleObj.Object;


	bReplicates = true;
	bReplicateMovement = true;


	health = 100;


	CollisionComp->InitSphereRadius(30.0f);
	CollisionComp->OnComponentHit.AddDynamic(this, &AMudBall::OnHit);


}



AMudBall::~AMudBall()
{

}

void AMudBall::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Hitsaaaaataatnna")));
}


void AMudBall::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (health <= 0)
		Explode();
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
