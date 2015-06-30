// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "UberWeihmacher.h"
#include "PlayerCharacter.h"


// Sets default values
AUberWeihmacher::AUberWeihmacher(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/RocketLauncher/RocketLauncher.RocketLauncher'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);

	weaponMesh->SetRelativeScale3D(FVector(1, 0.5, 1));

	reloadTime = 5;
	maxAmmo = 100;
	clips = 1;
	ammo = 100;
	ammoInClip = 100;
	pressure = 100;

	

	particleSystem = FOI.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_WaterSpray.P_WaterSpray'"));
	
	particleSystem->Template = ParticleObj.Object;

	
	
	particleSystem->AttachTo(weaponMesh, "SquirtSocket");

	particleSystem->SetFloatParameter("sprayVelocity", pressure);

	//particleSystem->even



 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	id = WEAPONID::WATER_GUN;
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