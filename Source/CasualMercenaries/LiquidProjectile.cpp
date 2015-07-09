// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "LiquidProjectile.h"


ALiquidProjectile::ALiquidProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//Movement
	ProjectileMovement->ProjectileGravityScale = 0.3;
	ProjectileMovement->InitialSpeed = 1600.f;


	//CollisionComponent
	CollisionComp->InitSphereRadius(15.0f);


	//Delegate
	OnActorHit.AddDynamic(this, &ALiquidProjectile::OnMyActorHit);


	//ParticleSystem
	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem>ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_MudSplash.P_MudSplash'"));
	flavorParticleEffect = ParticleObj.Object;


	//replication
	bReplicates = true;
	bReplicateMovement = true;
}

ALiquidProjectile::~ALiquidProjectile()
{

}

void ALiquidProjectile::OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	Splash();
}

void ALiquidProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ALiquidProjectile::BeginPlay()
{
	Super::BeginPlay();
}
bool ALiquidProjectile::Splash_Validate()
{
	return true;
}

void ALiquidProjectile::Splash_Implementation()
{	
	//Splashing
	UParticleSystemComponent *particle = UGameplayStatics::SpawnEmitterAtLocation(this, flavorParticleEffect, this->GetActorLocation(), FRotator::ZeroRotator, true);
	
	Destroy();
}