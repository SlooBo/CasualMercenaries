// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Granade.h"
#include "PlayerCharacter.h"


AGranade::AGranade(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	//StaticMesh
	projectileMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GranadeMesh"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/RocketLauncher/Rocket.Rocket'"));
	projectileMesh->SetStaticMesh(MeshObj.Object);
	//Material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/ToasterGun/MAT_toaster.MAT_toaster'"));
	projectileMesh->SetMaterial(0, MateriaObj.Object);
	//Scale
	projectileMesh->SetRelativeScale3D(FVector(.1, 1, 1));
	projectileMesh->SetSimulatePhysics(true);

	this->RootComponent = projectileMesh;


	//Movement
	ProjectileMovement->ProjectileGravityScale = 1.0;
	ProjectileMovement->InitialSpeed = 2000.f;


	//Integers
	lifeTime = 3;


	//Particles
	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_Explosion1.P_Explosion1'"));
	flavorParticleEffect = ParticleObj.Object;


	//Replication
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
	UParticleSystemComponent *particle = UGameplayStatics::SpawnEmitterAtLocation(this, flavorParticleEffect, this->GetActorLocation(), FRotator::ZeroRotator, true);
	particle->SetRelativeScale3D(FVector(2, 2, 2));
	float ExplosionRadius = 400.0f;
	float ExplosionDamage = 25.0f;

	for (TActorIterator<APlayerCharacter> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);
		
		if (distance <= ExplosionRadius)
		{
			//UGameplayStatics::ApplyDamage(*aItr, ExplosionDamage, GetInstigatorController(), this, UDamageType::StaticClass());
			APlayerCharacter* tempChar = Cast<APlayerCharacter>(this->GetOwner());
			aItr->ABaseCharacter::TakeDamage(ExplosionDamage, DAMAGE_TYPE::NORMAL, Cast<class APlayerController>(tempChar->GetController()));
		}
	}
	for (TActorIterator<AProjectile> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);

		if (distance <= ExplosionRadius)
		{
			//UGameplayStatics::ApplyDamage(*aItr, ExplosionDamage, GetInstigatorController(), this, UDamageType::StaticClass());
			AProjectile* tempChar = Cast<AProjectile>(this->GetOwner());
			aItr->TakeDamage(ExplosionDamage*2);
		}
	}
	Destroy();
}

void AGranade::BeginPlay()
{
	Super::BeginPlay();
}

//float AGranade::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
//{
//	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
//}