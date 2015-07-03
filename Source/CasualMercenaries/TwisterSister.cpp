// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "TwisterSister.h"
#include "Twister.h"
#include "PlayerCharacter.h"

ATwisterSister::ATwisterSister(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	//SkeletalMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/RocketLauncher/RocketLauncher.RocketLauncher'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);

	//Material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/ToasterGun/MAT_toaster.MAT_toaster'"));
	weaponMesh->SetMaterial(0, MateriaObj.Object);
	//Scale
	//weaponMesh->SetRelativeScale3D(FVector(0.05, 0.05, 0.05));

	//integer values
	maxAmmo = 120;
	clips = 999;
	ammo = 4;
	ammoInClip = 4;
	price = 600;

	//float values
	passedTimeReloading = 0;
	reloadTime = 5;
	firingInterval = 1;

	//MuzzleOffset
	//muzzleOffset.Z = -100;

	//particles
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_MachineGun_Muzzle.P_MachineGun_Muzzle'"));
	flavorParticleEffect = ParticleObj.Object;


	//const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj2(TEXT("ParticleSystem'/Game/Game/Particles/P_BulletTrail.P_BulletTrail'"));
	//bulletTrail = ParticleObj.Object;

	//ID
	id = WEAPONID::TWISTER_GUN;
	bReplicates = true;
}

void ATwisterSister::BeginPlay()
{
	Super::BeginPlay();
}

void ATwisterSister::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATwisterSister::PrimaryFunction(APlayerCharacter* user)
{
	firing = true;
}

void ATwisterSister::PrimaryFunctionReleased(APlayerCharacter* user)
{
	firing = false;
}

void ATwisterSister::SecondaryFunction(APlayerCharacter* user)
{

}

void ATwisterSister::Reload()
{

}

void ATwisterSister::Fire()
{
	FVector userLoc;
	FRotator cameraRot;

	this->GetOwner()->GetActorEyesViewPoint(userLoc, cameraRot);
	userLoc = this->GetOwner()->GetActorLocation();

	cameraRot.Pitch = 0;

	FVector const MuzzleLocation = userLoc + FTransform(cameraRot).TransformVector(muzzleOffset);
	muzzleOffset.X = 100;
	FRotator MuzzleRotation = cameraRot;

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this->GetOwner();
		SpawnParams.Instigator = Instigator;

		ATwister* const projectile = World->SpawnActor<ATwister>(ATwister::StaticClass(), MuzzleLocation, MuzzleRotation, SpawnParams);

		if (projectile)
		{
			FVector const LaunchDir = MuzzleRotation.Vector();
			projectile->InitVelocity(LaunchDir);
		}
	}
}

bool ATwisterSister::ServerEffect_Validate(UParticleSystem* particle, FVector location)
{
	return true;
}

void ATwisterSister::ServerEffect_Implementation(UParticleSystem* particle, FVector location)
{

}