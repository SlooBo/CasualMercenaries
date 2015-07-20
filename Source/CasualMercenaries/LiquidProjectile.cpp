// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "LiquidProjectile.h"
#include "PlayerCharacter.h"


ALiquidProjectile::ALiquidProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	projectileMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GranadeMesh"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Weapons/RocketLauncher/Rocket.Rocket'"));
	projectileMesh->SetStaticMesh(MeshObj.Object);
	//Material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/ToasterGun/MAT_toaster.MAT_toaster'"));
	projectileMesh->SetMaterial(0, MateriaObj.Object);
	//Scale
	projectileMesh->SetRelativeScale3D(FVector(.1, .1, .1));
	projectileMesh->SetSimulatePhysics(false);
	projectileMesh->SetVisibility(false);
	projectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	projectileMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


	//Movement
	ProjectileMovement->ProjectileGravityScale = 0.3;
	ProjectileMovement->InitialSpeed = 1600.f;


	//CollisionComponent
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);


	//Delegate
	OnActorHit.AddDynamic(this, &ALiquidProjectile::OnMyActorHit);


	//ParticleSystem
	const ConstructorHelpers::FObjectFinder<UParticleSystem>ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_WaterGun_Hit.P_WaterGun_Hit'"));
	
	flavorParticleEffect = ParticleObj.Object;


	particleSystem = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem>ParticleObj2(TEXT("ParticleSystem'/Game/Game/Particles/P_WaterGun_Shoot_Water.P_WaterGun_Shoot_Water'"));
	particleSystem->Template = ParticleObj2.Object;
	particleSystem->SetRelativeScale3D(FVector(40,40,40));
	particleSystem->AttachTo(projectileMesh, "ExhaustSocket");
	particleSystem->Activate();

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

	//replication
	bReplicates = true;
	bReplicateMovement = true;

	//particleSystem->BeginPlay();
	//particleSystem->trail
}

ALiquidProjectile::~ALiquidProjectile()
{

}

void ALiquidProjectile::OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!Cast<ALiquidProjectile>(OtherActor))
		if (!Cast<APlayerCharacter>(OtherActor))
			Splash();
	if (Cast<AGhostCharacter>(OtherActor))
		NormalImpulse = FVector::ZeroVector;
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
	particle->SetRelativeScale3D(FVector(4,4,4));
	Destroy();
}