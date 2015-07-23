// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Granade.h"
#include "PlayerCharacter.h"


AGranade::AGranade(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	//StaticMesh
	projectileMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GranadeMesh"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Props/Firehydrant/firehydrant.firehydrant'"));
	projectileMesh->SetStaticMesh(MeshObj.Object);
	//Material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Props/Firehydrant/MAT_firehydrant.MAT_firehydrant'"));
	projectileMesh->SetMaterial(0, MateriaObj.Object);
	//Scale
	projectileMesh->SetRelativeScale3D(FVector(.1, .1, .1));
	projectileMesh->SetSimulatePhysics(true);


	this->RootComponent = projectileMesh;

	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	//Movement
	ProjectileMovement->ProjectileGravityScale = 1.0;
	ProjectileMovement->InitialSpeed = 2000.f;


	//Delegate
	OnActorHit.AddDynamic(this, &AGranade::OnMyActorHit);


	//Integers
	lifeTime = 3;

	//Audio
	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	audioComp->SetVolumeMultiplier(0.525f);
	audioComp->bAutoActivate = false;
	audioComp->AttachParent = GetRootComponent();

	static ConstructorHelpers::FObjectFinder<USoundWave> audio1(TEXT("SoundWave'/Game/Game/Audio/AC_Hum_1.AC_Hum_1'"));
	if (audio1.Object)
		audioList.Add(audio1.Object);

	static ConstructorHelpers::FObjectFinder<USoundWave> audio2(TEXT("SoundWave'/Game/Game/Audio/Explosion_4.Explosion_4'"));
	if (audio2.Object)
		audioList.Add(audio2.Object);

	//Particles
	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_Explosion.P_Explosion'"));
	flavorParticleEffect = ParticleObj.Object;


	//Replication
	bReplicates = true;
	bReplicateMovement = true;

	//radial force
	radialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	radialForceComponent->ForceStrength = 50000;
	radialForceComponent->AttachTo(projectileMesh, "ExhaustSocket");
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

	audioComp->SetSound(audioList[1]);
	UGameplayStatics::PlaySoundAtLocation(this, audioComp->Sound, this->GetActorLocation(), 1, 1, 0, 0);
	radialForceComponent->FireImpulse();

	for (TActorIterator<APlayerCharacter> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);
		
		if (distance <= ExplosionRadius)
		{
			aItr->TakeDamage(ExplosionDamage, DAMAGE_TYPE::NORMAL, controller);
		}
	}
	for (TActorIterator<AProjectile> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);

		if (distance <= ExplosionRadius)
		{
			aItr->TakeDamage(ExplosionDamage*2);
		}
	}
	for (TActorIterator<ADestructibleObject> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);

		if (distance <= ExplosionRadius)
		{
			aItr->TakeDamage(ExplosionDamage * 2);
		}
	}

	Destroy();
}

void AGranade::BeginPlay()
{
	Super::BeginPlay();
}

void AGranade::OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	APlayerCharacter* pc = Cast<APlayerCharacter>(OtherActor);
	if (pc != nullptr && Cast<AGhostCharacter>(OtherActor) == nullptr)
		Explode();
}
