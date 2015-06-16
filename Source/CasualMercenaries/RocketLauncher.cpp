// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "RocketLauncher.h"
#include "PlayerCharacter.h"//this should not be working...
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

void ARocketLauncher::PrimaryFunction(APlayerCharacter* user)
{
	ammo--;

	FVector userLoc;
	FRotator cameraRot;

	user->GetActorEyesViewPoint(userLoc, cameraRot);

	userLoc = user->GetActorLocation();

	FVector const MuzzleLocation = userLoc + FTransform(cameraRot).TransformVector(MuzzleOffset);

	FRotator MuzzleRotation = cameraRot;


	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		// spawn the projectile at the muzzle
		AProjectile* const projectile = World->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);

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