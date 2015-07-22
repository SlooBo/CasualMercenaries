// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "PlayerCharacter.h"
#include "PomegranadeLauncher.h"
#include "DustBag.h"



APomeGranadeLauncher::APomeGranadeLauncher(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	//SkeletonMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/Weapons/ToasterGun/MESH_Toaster.MESH_Toaster'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);

	//Material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/ToasterGun/MAT_toaster.MAT_toaster'"));
	weaponMesh->SetMaterial(0, MateriaObj.Object);


	//integers
	maxAmmo = 6;
	clips = 999;
	ammo = 6;
	ammoInClip = 6;
	price = 900;

	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	audioComp->SetVolumeMultiplier(0.125f);
	audioComp->bAutoActivate = false;
	audioComp->AttachParent = GetRootComponent();

	static ConstructorHelpers::FObjectFinder<USoundWave> audio1(TEXT("SoundWave'/Game/Game/Audio/Grenadelauncher_Shoot.Grenadelauncher_Shoot'"));
	if (audio1.Object)
		audioList.Add(audio1.Object);

	static ConstructorHelpers::FObjectFinder<USoundWave> audio2(TEXT("SoundWave'/Game/Game/Audio/Reload_Magazine.Reload_Magazine'"));
	if (audio2.Object)
		audioList.Add(audio2.Object);

	static ConstructorHelpers::FObjectFinder<USoundWave> audio3(TEXT("SoundWave'/Game/Game/Audio/Grenadelauncher_Shoot.Grenadelauncher_Shoot'"));
	if (audio3.Object)
		audioList.Add(audio3.Object);


	//floats
	reloadTime = 2;
	firingInterval = 0.40;

	//ID
	id = WEAPONID::GRENADE_LAUNCHER;
	SuperFunctioAlaMiika();
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
	ammo--;
	audioComp->SetSound(audioList[0]);
	audioComp->Play();

	FVector userLoc;
	FRotator cameraRot;

	controller->GetPawn()->GetActorEyesViewPoint(userLoc, cameraRot);
	userLoc = controller->GetPawn()->GetActorLocation();

	FVector const MuzzleLocation = userLoc + FTransform(cameraRot).TransformVector(muzzleOffset);
	muzzleOffset.X = 100;
	FRotator MuzzleRotation = cameraRot;

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = controller->GetPawn();
		SpawnParams.Instigator = Instigator;

		AGranade* const projectile = World->SpawnActor<AGranade>(AGranade::StaticClass(), MuzzleLocation, MuzzleRotation, SpawnParams);

		if (projectile)
		{
			projectile->SetController(controller);
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

void APomeGranadeLauncher::Reload()
{
	reloading = true;
	audioComp->SetSound(audioList[1]);
	audioComp->Play();
}