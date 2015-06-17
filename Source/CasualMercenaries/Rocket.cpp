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
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/PrototypeParticleSystems/OfficeWars/ParticleSystems/PS_RocketTrail.PS_RocketTrail'"));
	
	particleSystem->Template = ParticleObj.Object;
	
	particleSystem->AttachTo(Mesh, "ExhaustSocket");

	particleSystem->Activate();
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
};