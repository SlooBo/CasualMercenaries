// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Rocket.h"
#include "PlayerCharacter.h"



ARocket::ARocket(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	//StaticMesh
	projectileMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RocketMesh"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Weapons/RocketLauncher/Rocket.Rocket'"));
	projectileMesh->SetStaticMesh(MeshObj.Object);
	//Material MISSING!


	//Movement
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0;


	//Delegate
	OnActorHit.AddDynamic(this, &ARocket::OnMyActorHit);


	//Stuff
	SetActorEnableCollision(true);

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
	particleSystem->Activate();


	//ParticleSystem2
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj2(TEXT("ParticleSystem'/Game/Game/Particles/P_Explosion.P_Explosion'"));
	flavorParticleEffect = ParticleObj2.Object;

	//radial force
	radialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	radialForceComponent->ForceStrength = 5000000;
	radialForceComponent->AttachTo(projectileMesh, "ExhaustSocket");

	//Replication
	bReplicates = true;
	bReplicateMovement = true;
}

void ARocket::OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this->GetOwner())
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

	float ExplosionRadius = 400.0f;
	float ExplosionDamage = 25.0f;
	//UGameplayStatics::ApplyRadialDamage(GetWorld(), 25, this->GetActorLocation(), 200, UDamageType::DamageFalloff(), this->GetOwner(), this->GetOwner(), );
	for (TActorIterator<APlayerCharacter> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);

		if (distance <= ExplosionRadius)
		{
			float x = 1;
			if (distance <= (ExplosionRadius / 2))
				;
			else
				x = 1 - (0.8 * (distance / ExplosionRadius));

			ExplosionDamage *= x;
			UGameplayStatics::ApplyDamage(*aItr, ExplosionDamage, GetInstigatorController(), this, UDamageType::StaticClass());
			APlayerCharacter* tempChar = Cast<APlayerCharacter>(this->GetOwner());
			aItr->TakeDamage(ExplosionDamage, DAMAGE_TYPE::NORMAL, Cast<class ACMPlayerController>(tempChar->GetController()));
		}
	}
	for (TActorIterator<AProjectile> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);

		if (distance <= ExplosionRadius)
		{
			//UGameplayStatics::ApplyDamage(*aItr, ExplosionDamage, GetInstigatorController(), this, UDamageType::StaticClass());
			AProjectile* tempChar = Cast<AProjectile>(this->GetOwner());
			aItr->TakeDamage(ExplosionDamage * 2);
		}
	}



	Destroy();
}
