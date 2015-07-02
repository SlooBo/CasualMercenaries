// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Twister.h"
#include "PlayerCharacter.h"


ATwister::ATwister(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	//Movement
	ProjectileMovement->InitialSpeed = 1200.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0;
	ProjectileMovement->Bounciness = 1.0f;

	//Delegate
	OnActorHit.AddDynamic(this, &ATwister::OnMyActorHit);


	//Stuff
	SetActorEnableCollision(true);


	//CollisionComponent
	CollisionComp->SetSphereRadius(30.0f);

	//ParticleSystem
	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_Tornado.P_Tornado'"));
	particleSystem->Template = ParticleObj.Object;
	particleSystem->AttachTo(projectileMesh, "ExhaustSocket");
	particleSystem->Activate();


	//ParticleSystem2
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj2(TEXT("ParticleSystem'/Game/Game/Particles/P_Explosion1.P_Explosion1'"));
	flavorParticleEffect = ParticleObj2.Object;


	//Replication
	bReplicates = true;
	bReplicateMovement = true;
}

void ATwister::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ATwister::BeginPlay()
{
	Super::BeginPlay();
}

void ATwister::OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

void ATwister::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Explode();
}
void ATwister::Explode()
{
	//do something"!!! 
}