// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Granade.h"


AGranade::AGranade(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	lifeTime = 3;


	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GranadeMesh"));

	this->RootComponent = Mesh;

	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Props/FIREHY~1/firehydrant.firehydrant'"));
	Mesh->SetStaticMesh(MeshObj.Object);

	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Props/FIREHY~1/MAT_firehydrant.MAT_firehydrant'"));
	Mesh->SetMaterial(0, MateriaObj.Object);

	Mesh->SetRelativeScale3D(FVector(0.05, 0.05, 0.05));
	Mesh->SetSimulatePhysics(true);


	ProjectileMovement->ProjectileGravityScale = 1.0;
	ProjectileMovement->InitialSpeed = 1000.f;


	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_Explosion1.P_Explosion1'"));

	part = ParticleObj.Object;


	bReplicates = true;
	bReplicateMovement = true;
}

AGranade::~AGranade()
{

}

void AGranade::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	livedTime += DeltaSeconds;
	if (livedTime >= lifeTime)
	{
		if (HasAuthority())
		Explode();
	}
}

bool AGranade::Explode_Validate()
{
	return true;
}

void AGranade::Explode_Implementation()
{
	UParticleSystemComponent *particle = UGameplayStatics::SpawnEmitterAtLocation(this, part, this->GetActorLocation(), FRotator::ZeroRotator, true);
	Destroy();
}

void AGranade::BeginPlay()
{

}

float AGranade::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}