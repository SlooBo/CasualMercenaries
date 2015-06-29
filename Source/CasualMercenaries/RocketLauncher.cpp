// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "RocketLauncher.h"
#include "PlayerCharacter.h"//this should not be working...
ARocketLauncher::ARocketLauncher(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/RocketLauncher/RocketLauncher.RocketLauncher'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);

	reloadTime = 1.5;
	maxAmmo = 6;
	clips = 999;
	ammo = 4;
	ammoInClip = 4;
	firingInterval = 0.50;

	PrimaryActorTick.bCanEverTick = true;

	this->SetActorHiddenInGame(true);

	bReplicates = true;

	particleSystem = FOI.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_RocketLauncher_MuzzleBack.P_RocketLauncher_MuzzleBack'"));

	particleSystem->Template = ParticleObj.Object;

	particleSystem->AttachTo(weaponMesh, "ExhaustSocket");
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
	if (ammo > 0)
	{
		firing = true;
	}
	else
	{	
		Reload();
	}
}

void ARocketLauncher::PrimaryFunctionReleased(APlayerCharacter* user)
{
	firing = false;
}
void ARocketLauncher::Reload()
{
	reloading = true;
}

void ARocketLauncher::Fire()
{
	if (ammo < 1)
	{
		firing = false;
		return;
	}
	ammo--;

	FVector userLoc;
	FRotator cameraRot;

	this->GetOwner()->GetActorEyesViewPoint(userLoc, cameraRot);

	userLoc = this->GetOwner()->GetActorLocation();
	MuzzleOffset.X = 100;
	FVector const MuzzleLocation = userLoc + FTransform(cameraRot).TransformVector(MuzzleOffset);
	
	FRotator MuzzleRotation = cameraRot;


	particleSystem->Activate();


	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this->GetOwner();
		SpawnParams.Instigator = Instigator;
		SpawnParams.bNoCollisionFail = true;


		// spawn the projectile at the muzzle
		ARocket* const projectile = World->SpawnActor<ARocket>(ARocket::StaticClass(), MuzzleLocation, MuzzleRotation, SpawnParams);

		if (projectile)
		{
			FVector const LaunchDir = MuzzleRotation.Vector();
			projectile->InitVelocity(LaunchDir);
		}
	}
}

void ARocketLauncher::SecondaryFunction(APlayerCharacter* user)
{

}