// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Rocket.h"
#include "PlayerCharacter.h"



ARocket::ARocket(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RocketMesh"));
	//Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
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

	SetActorEnableCollision(true);


	ProjectileMovement->InitialSpeed = 1200.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0;

	OnActorHit.AddDynamic(this, &ARocket::OnMyActorHit);
	
}

void ARocket::OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

void ARocket::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Explode();
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

void ARocket::Explode()
{
	UParticleSystemComponent *particle = UGameplayStatics::SpawnEmitterAtLocation(this, part, this->GetActorLocation(), FRotator::ZeroRotator, true);

	float ExplosionRadius = 200.0f;
	float ExplosionDamage = 25.0f;
	//UGameplayStatics::ApplyRadialDamage(GetWorld(), 25, this->GetActorLocation(), 200, UDamageType::DamageFalloff(), this->GetOwner(), this->GetOwner(), );
	for (TActorIterator<APlayerCharacter> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);

		if (distance <= ExplosionRadius)
		{
			//UGameplayStatics::ApplyDamage(*aItr, ExplosionDamage, GetInstigatorController(), this, UDamageType::StaticClass());
			//APlayerCharacter* tempChar = Cast<APlayerCharacter>(this->GetOwner());
			//aItr->TakeDamage(ExplosionDamage, Cast<APlayerController>(tempChar->GetController()));
		}
	}
	//Destroy();
}
