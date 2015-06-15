// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "RocketLauncher.h"

RocketLauncher::RocketLauncher(const FObjectInitializer& FOI)
{
	reloadTime = 1.5;
	maxAmmo = 6;
	clips = 5;
	ammo = 1;
	ammoInClip = 1;

	weaponMesh = FOI.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("RocketLauncherMesh"));
	weaponMesh->SetOnlyOwnerSee(false);
	weaponMesh->bCastDynamicShadow = true;
	weaponMesh->CastShadow = true;
}

RocketLauncher::~RocketLauncher()
{
}

void RocketLauncher::PrimaryFunction(AActor* user)
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

void RocketLauncher::SecondaryFunction()
{

}