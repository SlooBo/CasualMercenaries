// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "RocketLauncher.h"

ARocketLauncher::ARocketLauncher(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	reloadTime = 1.5;
	maxAmmo = 6;
	clips = 5;
	ammo = 1;
	ammoInClip = 1;

	PrimaryActorTick.bCanEverTick = true;
}

void ARocketLauncher::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ARocketLauncher::Tick(float DeltaTime)
{
	AWeapon::Tick(DeltaTime);

}

void ARocketLauncher::PrimaryFunction(AActor* user)
{
	ammo--;


	FVector CameraLoc;
	FRotator CameraRot;


	user->GetActorEyesViewPoint(CameraLoc, CameraRot);

	FVector const MuzzleLocation = CameraLoc + FTransform(CameraRot).TransformVector(MuzzleOffset);

	FRotator MuzzleRotation = CameraRot;
	MuzzleRotation.Pitch += 10.0f;

	FVector spawnLocation = user->GetActorLocation();

	FRotator spawnRotation = user->GetActorRotation();

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;


		AProjectile* const projectile = World->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
		// spawn the projectile at the muzzle
		if (projectile)
		{
			FVector const LaunchDir = MuzzleRotation.Vector();
			projectile->InitVelocity(LaunchDir);
		}
	}

}

void ARocketLauncher::SecondaryFunction()
{

}