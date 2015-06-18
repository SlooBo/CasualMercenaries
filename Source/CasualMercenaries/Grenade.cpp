// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Grenade.h"


AGrenade::AGrenade(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GranadeMesh"));


	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Props/FIREHY~1/firehydrant.firehydrant'"));
	Mesh->SetStaticMesh(MeshObj.Object);

	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Props/FIREHY~1/MAT_firehydrant.MAT_firehydrant'"));
	Mesh->SetMaterial(0, MateriaObj.Object);

	Mesh->SetRelativeScale3D(FVector(0.05, 0.05, 0.05));
	Mesh->SetSimulatePhysics(true);

	ProjectileMovement->ProjectileGravityScale = 1.0;

}

AGrenade::~AGrenade()
{

}

void AGrenade::Tick(float DeltaSeconds)
{

}
void AGrenade::BeginPlay()
{

}
