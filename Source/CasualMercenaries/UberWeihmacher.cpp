// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "UberWeihmacher.h"
#include "PlayerCharacter.h"


// Sets default values
AUberWeihmacher::AUberWeihmacher(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/RocketLauncher/RocketLauncher.RocketLauncher'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);

	reloadTime = 5;
	maxAmmo = 100;
	clips = 1;
	ammo = 100;
	ammoInClip = 100;

	

	particleSystem = FOI.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/WaterSpray/Game/Particles/P_WaterSpray.P_WaterSpray'"));
	
	particleSystem->Template = ParticleObj.Object;
	
	particleSystem->AttachTo(weaponMesh, "SquirtSocket");
	

	// tämä tulee laittaa primary funktioniin kunhan testaus on loppunut
	particleSystem->Activate();


 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AUberWeihmacher::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AUberWeihmacher::Tick( float DeltaTime )
{
	AWeapon::Tick(DeltaTime);
}

void AUberWeihmacher::PrimaryFunction(APlayerCharacter* user)
{

}

void AUberWeihmacher::SecondaryFunction()
{

}