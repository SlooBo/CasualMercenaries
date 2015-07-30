// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Twister.h"
#include "PlayerCharacter.h"
#include "Util.h"
#include "Granade.h"

ATwister::ATwister(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	//CollisionComponent
	RootComponent = CapsuleComp;
	CapsuleComp = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CapsuleComp"));
	CapsuleComp->InitCapsuleSize(100.0f, 200.0f);
	CapsuleComp->SetNotifyRigidBodyCollision(true);
	CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	//Movement
	ProjectileMovement->InitialSpeed = 1200.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0;
	ProjectileMovement->Bounciness = 1.0f;

	//Delegate
	OnActorHit.AddDynamic(this, &ATwister::OnMyActorHit);


	//Stuff
	SetActorEnableCollision(true);

	//Audio
	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	audioComp->SetVolumeMultiplier(0.525f);
	audioComp->bAutoActivate = false;

	static ConstructorHelpers::FObjectFinder<USoundWave> audio1(TEXT("SoundWave'/Game/Game/Audio/AC_Hum_1.AC_Hum_1'"));
	if (audio1.Object)
		audioList.Add(audio1.Object);

	static ConstructorHelpers::FObjectFinder<USoundWave> audio2(TEXT("SoundWave'/Game/Game/Audio/Explosion_4.Explosion_4'"));
	if (audio2.Object)
		audioList.Add(audio2.Object);

	//lifeTime
	deathTime = 20;

	//ParticleSystem
	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_Tornado.P_Tornado'"));
	particleSystem->Template = ParticleObj.Object;
	particleSystem->Activate();
	particleSystem->SetRelativeScale3D(FVector(4.0f, 4.0f, 2.0f));
	particleSystem->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));

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

	timer = 0;
}

void ATwister::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
		return;

	timer += DeltaSeconds;

	if (timer > .25)
	{
		const float radius = 300;
		const float damage = 10;
		const float mindamage = 2;
		const float force = 2750;

		for (TActorIterator<APlayerCharacter> aItr(GetWorld()); aItr; ++aItr)
		{
			float distance = Util::GetDistanceToPlayer(this, *aItr);

			if (distance <= radius)
			{
				float multiplier = AProjectile::CalculateExplosionDamageMultiplier(damage, distance, radius, mindamage, 0.0f);
				float finalDamage = damage*multiplier;

				UGameplayStatics::ApplyDamage(*aItr, finalDamage, GetInstigatorController(), this, UDamageType::StaticClass());
				aItr->TakeDamage(finalDamage, DAMAGE_TYPE::NORMAL, Cast<class ACMPlayerController>(controller));

				// apply some lift off for the player to get them off the ground
				FVector liftoffLoc = aItr->GetActorLocation();
				liftoffLoc.Z -= aItr->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 15.0f;
				aItr->GetMovementComponent()->AddRadialImpulse(liftoffLoc, radius*0.6f, force*0.3f, ERadialImpulseFalloff::RIF_Linear, true);

				aItr->GetMovementComponent()->AddRadialImpulse(GetActorLocation(), radius, force, ERadialImpulseFalloff::RIF_Linear, true);

				
			}

		}

		// throws other colliding projectiles around 
		for (TActorIterator<AProjectile> aItr(GetWorld()); aItr; ++aItr)
		{
			if (Cast<ATwister>(*aItr) != NULL)
				continue;

			float distance = GetDistanceTo(*aItr);
			if (distance <= radius)
			{
				FVector velocity((FMath::FRand()*2)-1, (FMath::FRand()*2)-1, (FMath::FRand()*2)-1);
				velocity = velocity.GetSafeNormal();

				aItr->InitVelocity(velocity);
			}
		}

		//radialForceComponent->FireImpulse();
		timer = 0;
	}


}

void ATwister::BeginPlay()
{
	Super::BeginPlay();
}

void ATwister::OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	FlipShitUp();
}

void ATwister::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	
}
void ATwister::FlipShitUp()
{

}