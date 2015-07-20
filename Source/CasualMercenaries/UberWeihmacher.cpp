// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "UberWeihmacher.h"
#include "LiquidProjectile.h"
#include "PlayerCharacter.h"


// Sets default values
AUberWeihmacher::AUberWeihmacher(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	//SkeletonMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/Weapons/WaterGun/MESH_Watergun.MESH_Watergun'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);
	//Material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/WaterGun/MAT_watergun.MAT_watergun'"));
	weaponMesh->SetMaterial(0, MateriaObj.Object);

	//Scaling

	//Integers
	maxAmmo = 100;
	clips = 1;
	ammo = 100;
	ammoInClip = 100;
	pressure = 100;
	price = 1500;

	//Floats
	reloadTime = 5;

	//Audio
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

	//ParticleSystem
	particleSystem = FOI.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_WaterSpray.P_WaterSpray'"));
	particleSystem->Template = ParticleObj.Object;
	particleSystem->AttachTo(weaponMesh, "ExhaustSocket");


	particleSystem->SetFloatParameter("sprayVelocity", pressure);


	//ID
	id = WEAPONID::WATER_GUN;
	SuperFunctioAlaMiika();
	//replication
	bReplicates = true;
}

// Called when the game starts or when spawned
void AUberWeihmacher::BeginPlay()
{
	Super::BeginPlay();
	particleSystem->Deactivate();
}

// Called every frame
void AUberWeihmacher::Tick( float DeltaTime )
{
	AWeapon::Tick(DeltaTime);
}

void AUberWeihmacher::PrimaryFunction(APlayerCharacter* user)
{
	// souund cue for firing
	audioComp->SetSound(audioList[0]);
	audioComp->Play();
	firing = true;
}

void AUberWeihmacher::Fire()
{
	//TODO MAKE THE FIRING GO MORE TOWARDS THE TARGETING RETICULE
	ammo--;


	// complete briefing on the workings of spawning a projectile found in RocketLauncher.cpp

	FVector userLoc;
	FRotator cameraRot;

	controller->GetPawn()->GetActorEyesViewPoint(userLoc, cameraRot);

	userLoc = controller->GetPawn()->GetActorLocation();
	muzzleOffset.X = 100;
	FVector const MuzzleLocation = userLoc + FTransform(cameraRot).TransformVector(muzzleOffset);


	FRotator MuzzleRotation = cameraRot;


	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = controller->GetPawn();
		SpawnParams.Instigator = Instigator;
		SpawnParams.bNoCollisionFail = true;


		// spawn the projectile at the muzzle
		ALiquidProjectile* const liquidProjectile = World->SpawnActor<ALiquidProjectile>(ALiquidProjectile::StaticClass(), weaponMesh->GetSocketLocation("ExhaustSocket"), MuzzleRotation, SpawnParams);

		if (liquidProjectile)
		{
			FVector const LaunchDir = MuzzleRotation.Vector();
			liquidProjectile->InitVelocity(LaunchDir);
		}
	}
}

void AUberWeihmacher::PrimaryFunctionReleased(APlayerCharacter* user)
{
	particleSystem->Deactivate();
	firing = false;
}

void AUberWeihmacher::SecondaryFunction(APlayerCharacter* user)
{
	pressure += 5;
}