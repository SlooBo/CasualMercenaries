// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "MudBall.h"
#include "PlayerCharacter.h"


AMudBall::AMudBall(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//StaticMesh
	projectileMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GranadeMesh"));
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("StaticMesh'/Game/Game/Weapons/MudBuster/Projectile/MESH_Budbuster_projectile.MESH_Budbuster_projectile'"));
	projectileMesh->SetStaticMesh(MeshObj.Object);

	//Material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/MudBuster/Projectile/MAT_Mudbuster_projectile.MAT_Mudbuster_projectile'")); // Material missing!!!!!
	projectileMesh->SetMaterial(0, MateriaObj.Object);

	//NavMeshIntegration
	projectileMesh->SetCanEverAffectNavigation(true);
	projectileMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	
	//Movement
	ProjectileMovement->ProjectileGravityScale = 0.1;
	ProjectileMovement->InitialSpeed = 2500.f;


	//CollisionComponent
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	//Delegate
	OnActorHit.AddDynamic(this, &AMudBall::OnMyActorHit);


	//Integers
	health = 100;
	size = 0.2;

	//Scale
	projectileMesh->SetRelativeScale3D(FVector(size, size, size));

	//Triggers
	inflating = false;


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
	const ConstructorHelpers::FObjectFinder<UParticleSystem>ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_Mud_Destroy.P_Mud_Destroy'"));
	flavorParticleEffect = ParticleObj.Object;


	//replication
	bReplicates = true;
	bReplicateMovement = true;
}

AMudBall::~AMudBall()
{

}

void AMudBall::OnMyActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor)
	{
	}
	else
	{
		ProjectileMovement->SetActive(false, false);
		inflating = true;
	}
}

void AMudBall::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!HasAuthority())
		return;
		if (health <= 0)	
			Explode();

	if (inflating)
	{		
		if (size > 1)
		{	
			inflating = false;
			return;
		}
		Expand();
	}
}

void AMudBall::BeginPlay()
{
	Super::BeginPlay();
}

bool AMudBall::Explode_Validate()
{
	return true;
}

void AMudBall::Explode_Implementation()
{
	UParticleSystemComponent *particle = UGameplayStatics::SpawnEmitterAtLocation(this, flavorParticleEffect, this->GetActorLocation(), FRotator::ZeroRotator, true);
	particle->SetRelativeScale3D(FVector(2, 2, 2));
	Destroy();
}

bool AMudBall::Expand_Validate()
{
	return true;
}

void AMudBall::Expand_Implementation()
{
	size += 0.1;
	projectileMesh->SetRelativeScale3D(FVector(size, size, size));
}