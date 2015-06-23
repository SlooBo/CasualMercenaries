// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Rocket.h"



ARocket::ARocket(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RocketMesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/RocketLauncher/Rocket.Rocket'"));
	Mesh->SetStaticMesh(MeshObj.Object);


	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_SmokeTrail.P_SmokeTrail'"));
	
	particleSystem->Template = ParticleObj.Object;
	
	particleSystem->AttachTo(Mesh, "ExhaustSocket");

	particleSystem->Activate();

	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj2(TEXT("ParticleSystem'/Game/Game/Particles/P_Explosion1.P_Explosion1'"));
	part = ParticleObj2.Object;
	bReplicates = true;
	bReplicateMovement = true;
}

ARocket::~ARocket()
{

}

void ARocket::Tick(float DeltaSeconds)
{

}

void ARocket::BeginPlay()
{
	particleSystem->BeginPlay();
}

void ARocket::Explode(const FObjectInitializer& ObjectInitializer)
{
	UParticleSystemComponent *particle = UGameplayStatics::SpawnEmitterAtLocation(this, part, this->GetActorLocation(), FRotator::ZeroRotator, true);
	Destroy();
}
