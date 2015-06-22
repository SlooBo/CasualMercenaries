// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Granade.h"


AGranade::AGranade(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GranadeMesh"));

	this->RootComponent = Mesh;

	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Props/FIREHY~1/firehydrant.firehydrant'"));
	Mesh->SetStaticMesh(MeshObj.Object);

	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Props/FIREHY~1/MAT_firehydrant.MAT_firehydrant'"));
	Mesh->SetMaterial(0, MateriaObj.Object);

	Mesh->SetRelativeScale3D(FVector(0.05, 0.05, 0.05));
	Mesh->SetSimulatePhysics(true);

	ProjectileMovement->ProjectileGravityScale = 1.0;

	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));

	particleSystem->Deactivate();
}

AGranade::~AGranade()
{

}

void AGranade::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AGranade::OnCollision);
}

void AGranade::BeginPlay()
{

}


//void AGranade::OnCollision(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//
//
//	//Super::OnCollision(OtherActor, OtherComp,OtherBodyIndex, bFromSweep,SweepResult);
//	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
//	{
//		// do some stuff
//		const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_Explosion1.P_Explosion1'"));
//
//		particleSystem->Template = ParticleObj.Object;
//
//		particleSystem->AttachTo(Mesh, "ExhaustSocket");
//		particleSystem->Activate();
//
//	}
//}

float AGranade::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_Explosion1.P_Explosion1'"));

	particleSystem->Template = ParticleObj.Object;

	particleSystem->AttachTo(Mesh, "ExhaustSocket");
	particleSystem->Activate();

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}