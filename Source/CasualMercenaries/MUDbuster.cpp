// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "MUDbuster.h"
#include "MudBall.h"




AMUDbuster::AMUDbuster(const FObjectInitializer& FOI) : Super(FOI)
{

	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/Weapons/MudBuster/Weapon/MESH_Mudbuster.MESH_Mudbuster'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);

	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/MudBuster/Weapon/MAT_Mudbuster.MAT_Mudbuster'"));
	weaponMesh->SetMaterial(0, MateriaObj.Object);


	weaponMesh->SetRelativeScale3D(FVector(0.5, 0.5, 0.25));

	bReplicates = true;

	reloadTime = 0.5;
	maxAmmo = 120;
	clips = 999;
	ammo = 10;
	ammoInClip = 10;
	firingInterval = .75;
	price = 1200;

	passedTimeReloading = 0;

	id = WEAPONID::MUDBUSTER_GUN;

	//weaponMesh->AnimScriptInstance
}

void AMUDbuster::PrimaryFunction(APlayerCharacter* user)
{
	firing = true;
}

void AMUDbuster::PrimaryFunctionReleased(APlayerCharacter* user)
{
	firing = false;
}

void AMUDbuster::SecondaryFunction(APlayerCharacter* user)
{

}

void AMUDbuster::BeginPlay()
{
	Super::BeginPlay();
}

void AMUDbuster::Reload()
{

}



void AMUDbuster::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


void AMUDbuster::Fire()
{
	firing = false;

	FVector userLoc;
	FRotator cameraRot;

	this->GetOwner()->GetActorEyesViewPoint(userLoc, cameraRot);

	userLoc = this->GetOwner()->GetActorLocation();
	MuzzleOffset.X = 100;
	FVector const MuzzleLocation = userLoc + FTransform(cameraRot).TransformVector(MuzzleOffset);

	FRotator MuzzleRotation = cameraRot;


	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this->GetOwner();
		SpawnParams.Instigator = Instigator;
		SpawnParams.bNoCollisionFail = true;


		// spawn the projectile at the muzzle
		AMudBall* const projectile = World->SpawnActor<AMudBall>(AMudBall::StaticClass(), MuzzleLocation, MuzzleRotation, SpawnParams);

		if (projectile)
		{
			FVector const LaunchDir = MuzzleRotation.Vector();
			projectile->InitVelocity(LaunchDir);
		}
	}
}