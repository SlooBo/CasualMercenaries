// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "UberWeihmacher.h"
#include "PlayerCharacter.h"


// Sets default values
AUberWeihmacher::AUberWeihmacher(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	//SkeletonMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/RocketLauncher/RocketLauncher.RocketLauncher'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);
	//Material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/WaterGun/MAT_watergun.MAT_watergun'"));
	weaponMesh->SetMaterial(0, MateriaObj.Object);

	//Scaling
	weaponMesh->SetRelativeScale3D(FVector(1, 1, 1));

	//Integers
	maxAmmo = 100;
	clips = 1;
	ammo = 100;
	ammoInClip = 100;
	pressure = 100;
	price = 1500;

	//Floats
	reloadTime = 5;

	//ParticleSystem
	particleSystem = FOI.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_WaterSpray.P_WaterSpray'"));
	
	particleSystem->Template = ParticleObj.Object;
	particleSystem->AttachTo(weaponMesh, "SquirtSocket");
	particleSystem->SetFloatParameter("sprayVelocity", pressure);


	//ID
	id = WEAPONID::WATER_GUN;

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
	particleSystem->SetFloatParameter("sprayVelocity", 500);
	particleSystem->SetFloatParameter("sprayAmmount", 500);
	particleSystem->Activate();
}
void AUberWeihmacher::PrimaryFunctionReleased(APlayerCharacter* user)
{
	particleSystem->Deactivate();
}

void AUberWeihmacher::SecondaryFunction(APlayerCharacter* user)
{
	pressure += 5;
}