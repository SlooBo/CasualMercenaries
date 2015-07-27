// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Projectile.h"
#include "GhostCharacter.h"


// Sets default values
AProjectile::AProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//CollisionComponent
	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->SetCollisionObjectType(ECC_Projectile);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));

	//Set Root component
	RootComponent = CollisionComp;

	//MovementComponent  
	ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 600.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
	ProjectileMovement->ProjectileGravityScale = 0.1;

	lifeTime = 0;
	deathTime = 300;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	controller = Cast<ACMPlayerController>(GetOwner());
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every second checking if actor should be replicated to target
bool AProjectile::IsNetRelevantFor(const AActor* realViewer, const AActor* viewTarget, const FVector& srcLocation) const
{
	bool relevant = Super::IsNetRelevantFor(realViewer, viewTarget, srcLocation);

	// deny actor updates for ghost characters
	if (viewTarget != this && Cast<AGhostCharacter>(viewTarget) != NULL)
		relevant = false;

	return relevant;
}

// Called every frame
void AProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	lifeTime += DeltaTime;
	if (lifeTime >= deathTime)
		Destroy();
}

void AProjectile::InitVelocity(const FVector& ShootDirection)
{
	if (ProjectileMovement)
	{
		// set the projectile's velocity to the desired direction
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
	}
}


