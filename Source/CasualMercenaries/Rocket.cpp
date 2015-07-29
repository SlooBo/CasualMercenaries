// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Rocket.h"
#include "PlayerCharacter.h"
#include "Twister.h"
#include "DestructibleObject.h"
#include "Util.h"


ARocket::ARocket(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	//StaticMesh
	projectileMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RocketMesh"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Weapons/RocketLauncher/Rocket.Rocket'"));
	projectileMesh->SetStaticMesh(MeshObj.Object);
	projectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Material MISSING!

	//Movement
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0;

	//Delegate
	OnActorHit.AddDynamic(this, &ARocket::OnMyActorHit);

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

	//ParticleSystem
	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_Rocket_SmokeTrail.P_Rocket_SmokeTrail'"));
	particleSystem->Template = ParticleObj.Object;
	particleSystem->AttachTo(projectileMesh, "ExhaustSocket");
	particleSystem->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	particleSystem->Activate();


	//ParticleSystem2
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj2(TEXT("ParticleSystem'/Game/Game/Particles/P_Explosion.P_Explosion'"));
	flavorParticleEffect = ParticleObj2.Object;

	//radial force
	radialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	radialForceComponent->ForceStrength = 50000;
	radialForceComponent->AttachTo(projectileMesh, "ExhaustSocket");

	//Replication
	bReplicates = true;
	bReplicateMovement = true;

	asd = 0;
	casd = false;
}

void ARocket::OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Cast<ATwister>(OtherActor) || Cast<AGhostCharacter>(OtherActor))
		return;

	if (Cast<APlayerCharacter>(OtherActor))
		directHitPlayer = OtherActor;

	Explode();
}

void ARocket::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Explode();
}

ARocket::~ARocket()
{

}

void ARocket::Tick(float DeltaSeconds)
{
	asd += DeltaSeconds;
	if (!casd)
	{
		if (asd > 1)
		{
			casd = true;
			CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
			projectileMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		}
	}
}

void ARocket::BeginPlay()
{
	particleSystem->BeginPlay();
}

void ARocket::Explode()
{
	UParticleSystemComponent *particle = UGameplayStatics::SpawnEmitterAtLocation(this, flavorParticleEffect, this->GetActorLocation(), FRotator::ZeroRotator, true);

	audioComp->SetSound(audioList[1]);

	UGameplayStatics::PlaySoundAtLocation(this, audioComp->Sound, this->GetActorLocation(), 1, 1, -0.50f, 0);
	radialForceComponent->FireImpulse();

	if (Role >= ROLE_Authority)
	{
		const float ExplosionRadius = 400.0f;
		const float ExplosionFullDamageDistance = ExplosionRadius * 0.1f;
		const float ExplosionDamage = 40.0f;
		const float ExplosionMinDamage = 15.0f;

		for (TActorIterator<APlayerCharacter> aItr(GetWorld()); aItr; ++aItr)
		{
			float distance = Util::GetDistanceToPlayer(this, *aItr);

			if (distance <= ExplosionRadius)
			{
				float multiplier = AProjectile::CalculateExplosionDamageMultiplier(ExplosionDamage, distance, ExplosionRadius, ExplosionMinDamage, ExplosionFullDamageDistance);
				if (directHitPlayer == *aItr)
					multiplier = 1.0f;

				float finalDamage = ExplosionDamage*multiplier;

				UGameplayStatics::ApplyDamage(*aItr, finalDamage, GetInstigatorController(), this, UDamageType::StaticClass());
				aItr->TakeDamage(finalDamage, DAMAGE_TYPE::NORMAL, Cast<class ACMPlayerController>(controller));
			}
		}
		for (TActorIterator<AProjectile> aItr(GetWorld()); aItr; ++aItr)
		{
			float distance = GetDistanceTo(*aItr);

			if (distance <= ExplosionRadius)
			{
				float multiplier = AProjectile::CalculateExplosionDamageMultiplier(ExplosionDamage, distance, ExplosionRadius, ExplosionMinDamage, ExplosionFullDamageDistance);
				float finalDamage = ExplosionDamage*multiplier;
				aItr->TakeDamage(finalDamage * 2);
			}
		}
		for (TActorIterator<ADestructibleObject> aItr(GetWorld()); aItr; ++aItr)
		{
			float distance = GetDistanceTo(*aItr);

			if (distance <= ExplosionRadius)
			{
				float multiplier = AProjectile::CalculateExplosionDamageMultiplier(ExplosionDamage, distance, ExplosionRadius, ExplosionMinDamage, ExplosionFullDamageDistance);
				float finalDamage = ExplosionDamage*multiplier;
				aItr->TakeDamage(finalDamage * 2);
			}
		}
	}

	Destroy();
}
