// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "PlayerCharacter.h"
#include "PomegranadeLauncher.h"




APomeGranadeLauncher::APomeGranadeLauncher(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	//SkeletonMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/ToasterGun/MESH_Toaster_gun.MESH_Toaster_gun'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);

	//Material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/ToasterGun/MAT_toaster.MAT_toaster'"));
	weaponMesh->SetMaterial(0, MateriaObj.Object);

	//Scaling
	weaponMesh->SetRelativeScale3D(FVector(0.05, 0.05, 0.05));

	//integers
	maxAmmo = 6;
	clips = 999;
	ammo = 6;
	ammoInClip = 6;
	price = 900;


	//floats
	reloadTime = 2;
	firingInterval = 0.40;

	//ID
	id = WEAPONID::GRENADE_LAUNCHER;

	//replication
	bReplicates = true;
}

void APomeGranadeLauncher::PrimaryFunction(APlayerCharacter* user)
{
	this->SetOwner(user);
	if (ammo > 0)
		firing = true;
}

void APomeGranadeLauncher::Fire()
{
	FVector userLoc;
	FRotator cameraRot;

	this->GetOwner()->GetActorEyesViewPoint(userLoc, cameraRot);
	userLoc = this->GetOwner()->GetActorLocation();

	FVector const MuzzleLocation = userLoc + FTransform(cameraRot).TransformVector(muzzleOffset);
	muzzleOffset.X = 100;
	FRotator MuzzleRotation = cameraRot;

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this->GetOwner();
		SpawnParams.Instigator = Instigator;

		AGranade* const projectile = World->SpawnActor<AGranade>(AGranade::StaticClass(), MuzzleLocation, MuzzleRotation, SpawnParams);

		if (projectile)
		{
			FVector const LaunchDir = MuzzleRotation.Vector();
			projectile->InitVelocity(LaunchDir);
		}
	}
}

void APomeGranadeLauncher::PrimaryFunctionReleased(APlayerCharacter* user)
{
	firing = false;
}

void APomeGranadeLauncher::SecondaryFunction(APlayerCharacter* user)
{

}

void APomeGranadeLauncher::BeginPlay()
{
	Super::BeginPlay();
}

void APomeGranadeLauncher::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
