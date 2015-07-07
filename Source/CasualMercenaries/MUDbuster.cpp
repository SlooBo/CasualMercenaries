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

	//Audio
	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	audioComp->SetVolumeMultiplier(0.125f);
	audioComp->bAutoActivate = false;

	static ConstructorHelpers::FObjectFinder<USoundWave> audio1(TEXT("SoundWave'/Game/Game/Audio/Grenadelauncher_Shoot.Grenadelauncher_Shoot'"));
	if (audio1.Object)
		audioList.Add(audio1.Object);

	static ConstructorHelpers::FObjectFinder<USoundWave> audio2(TEXT("SoundWave'/Game/Game/Audio/Reload_Magazine.Reload_Magazine'"));
	if (audio2.Object)
		audioList.Add(audio2.Object);

	static ConstructorHelpers::FObjectFinder<USoundWave> audio3(TEXT("SoundWave'/Game/Game/Audio/Grenadelauncher_Shoot.Grenadelauncher_Shoot'"));
	if (audio3.Object)
		audioList.Add(audio3.Object);

	// floats
	firingInterval = .75;
	reloadTime = 0.5;
	passedTimeReloading = 0;

	//ID
	id = WEAPONID::MUDBUSTER_GUN;
	SuperFunctioAlaMiika();
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
	audioComp->SetSound(audioList[1]);
	audioComp->Play();
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