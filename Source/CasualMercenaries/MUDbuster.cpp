// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "MUDbuster.h"
#include "MudBall.h"




AMUDbuster::AMUDbuster(const FObjectInitializer& FOI) : Super(FOI)
{
	//skeletalMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/Weapons/MudBuster/Weapon/MESH_Mudbuster.MESH_Mudbuster'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);

	//skeletalMesh
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/MudBuster/Weapon/MAT_Mudbuster.MAT_Mudbuster'"));
	weaponMesh->SetMaterial(0, MateriaObj.Object);
	weaponMesh->SetRelativeScale3D(FVector(0.5, 0.5, 0.25));

	// integers
	maxAmmo = 120;
	clips = 999;
	ammo = 10;
	ammoInClip = 10;
	price = 1200;

	// floats
	firingInterval = .75;
	reloadTime = 0.5;
	passedTimeReloading = 0;

	//ID
	id = WEAPONID::MUDBUSTER_GUN;

	//replication
	bReplicates = true;
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

	//Get & set the orientation of projectile to be created
	FVector tempUserLoc;
	FRotator tempCameraRot;

	this->GetOwner()->GetActorEyesViewPoint(tempUserLoc, tempCameraRot);
	tempUserLoc = this->GetOwner()->GetActorLocation();

	muzzleOffset.X = 100;
	FVector const muzzleLocation = tempUserLoc + FTransform(tempCameraRot).TransformVector(muzzleOffset);
	FRotator tempMuzzleRotation = tempCameraRot;


	
	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this->GetOwner();
		SpawnParams.Instigator = Instigator;
		SpawnParams.bNoCollisionFail = true;


		// spawn the projectile at the muzzle
		AMudBall* const projectile = World->SpawnActor<AMudBall>(AMudBall::StaticClass(), muzzleLocation, tempMuzzleRotation, SpawnParams);

		if (projectile)
		{
			FVector const tempLaunchDir = tempMuzzleRotation.Vector();
			projectile->InitVelocity(tempLaunchDir);
		}
	}
}