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
	clips = 5;
	ammo = 1;
	ammoInClip = 1;

	PrimaryActorTick.bCanEverTick = true;

	this->SetActorHiddenInGame(true);

	bReplicates = true;
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
	MuzzleOffset.X = 100;
	FVector const MuzzleLocation = userLoc + FTransform(cameraRot).TransformVector(MuzzleOffset);
	
	FRotator MuzzleRotation = cameraRot;

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = user;
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

void ARocketLauncher::SecondaryFunction()
{

}