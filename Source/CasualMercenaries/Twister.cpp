// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Twister.h"
#include "PlayerCharacter.h"


ATwister::ATwister(const FObjectInitializer& ObjectInitializer) : AProjectile(ObjectInitializer)
{
	//CollisionComponent
	RootComponent = CapsuleComp;
	CapsuleComp = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CapsuleComp"));
	CapsuleComp->InitCapsuleSize(100.0f, 200.0f);
	CapsuleComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CapsuleComp->SetNotifyRigidBodyCollision(true);


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




	//ParticleSystem
	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_Tornado.P_Tornado'"));
	particleSystem->Template = ParticleObj.Object;
	//particleSystem->AttachTo(projectileMesh, "ExhaustSocket");
	particleSystem->Activate();
	particleSystem->SetRelativeScale3D(FVector(4.0f, 4.0f, 1.0f));
	particleSystem->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));

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
	FlipShitUp();
}

void ATwister::OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProjectile* projectile = Cast<AProjectile>(OtherActor);
	if (projectile)
	{
		float random1 = FMath::FRand();
		float random2 = FMath::FRand();
		float random3 = FMath::FRand();

		FVector velocity = projectile->GetVelocity();
		velocity.X *= random1;
		velocity.Y *= random2;
		velocity.Z *= random3;
		projectile->InitVelocity(FVector(random1 ,random2, random3) * 1000);
	}

}
void ATwister::FlipShitUp()
{
	


}