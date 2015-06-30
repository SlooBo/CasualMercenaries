
// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Granade.h"
#include "PlayerCharacter.h"


AGranade::AGranade(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	lifeTime = 3;


	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GranadeMesh"));

	this->RootComponent = Mesh;

	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/RocketLauncher/Rocket.Rocket'"));
	Mesh->SetStaticMesh(MeshObj.Object);

	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/ToasterGun/MAT_toaster.MAT_toaster'"));
	Mesh->SetMaterial(0, MateriaObj.Object);

	Mesh->SetRelativeScale3D(FVector(.1, 1, 1));
	Mesh->SetSimulatePhysics(true);


	ProjectileMovement->ProjectileGravityScale = 1.0;
	ProjectileMovement->InitialSpeed = 2000.f;


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

	float ExplosionRadius = 400.0f;
	float ExplosionDamage = 25.0f;

	for (TActorIterator<APlayerCharacter> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);
		
		if (distance <= ExplosionRadius)
		{
			//UGameplayStatics::ApplyDamage(*aItr, ExplosionDamage, GetInstigatorController(), this, UDamageType::StaticClass());
			APlayerCharacter* tempChar = Cast<APlayerCharacter>(this->GetOwner());
			aItr->TakeDamage(ExplosionDamage, Cast<APlayerController>(tempChar->GetController()));
		}
	}
	for (TActorIterator<AProjectile> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);

		if (distance <= ExplosionRadius)
		{
			//UGameplayStatics::ApplyDamage(*aItr, ExplosionDamage, GetInstigatorController(), this, UDamageType::StaticClass());
			AProjectile* tempChar = Cast<AProjectile>(this->GetOwner());
			aItr->TakeDamage(ExplosionDamage);
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