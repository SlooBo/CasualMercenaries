// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "WaspNestCudgel.h"
#include "PlayerCharacter.h"




AWaspNestCudgel::AWaspNestCudgel(const FObjectInitializer& FOI) : Super(FOI)
{
	//SkeletonMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/Weapons/Beehive/MESH_Beehive.MESH_Beehive'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);
	//Material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/MudBuster/Weapon/MAT_Mudbuster.MAT_Mudbuster'")); // Material missing!!!!!
	weaponMesh->SetMaterial(0, MateriaObj.Object);
	//Scaling


	//Audio
	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	audioComp->SetVolumeMultiplier(0.125f);
	audioComp->bAutoActivate = false;
	audioComp->AttachParent = GetRootComponent();

	//the followiing sound files should go as follows
	// - firing souund
	// - reloading sound
	// - other needed sounds

	//loading of an audiosnippet
	static ConstructorHelpers::FObjectFinder<USoundWave> audio1(TEXT("SoundWave'/Game/Game/Audio/Bees.Bees'"));
	if (audio1.Object)
		audioList.Add(audio1.Object);

	//loading of an audiosnippet
	static ConstructorHelpers::FObjectFinder<USoundWave> audio2(TEXT("SoundWave'/Game/Game/Audio/Reload_Magazine.Reload_Magazine'"));
	if (audio2.Object)
		audioList.Add(audio2.Object);

	//loading of an audiosnippet
	static ConstructorHelpers::FObjectFinder<USoundWave> audio3(TEXT("SoundWave'/Game/Game/Audio/Grenadelauncher_Shoot.Grenadelauncher_Shoot'"));
	if (audio3.Object)
		audioList.Add(audio3.Object);


	//ParticleSystem
	particleSystem = FOI.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Wasps"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_Beehive.P_Beehive'"));
	particleSystem->Template = ParticleObj.Object;
	particleSystem->AttachTo(weaponMesh, "WaspSocket");
	particleSystem->SetRelativeScale3D(FVector(10, 10, 10));


	//Integers
	maxAmmo = 120;
	clips = 999;
	ammo = 100;
	ammoInClip = 100;


	//Floats
	price = 500;
	reloadTime = 2.5;
	passedTimeReloading = 0;
	firingInterval = .5;


	//ID
	id = WEAPONID::WASP_GUN;
	SuperFunctioAlaMiika();


	//replication
	bReplicates = true;
}

void AWaspNestCudgel::PrimaryFunction(APlayerCharacter* user)
{
	particleSystem->Activate();
	// sets firing true and plays sound
	firing = true;
	audioComp->SetSound(audioList[0]);
	audioComp->Play();
}

void AWaspNestCudgel::PrimaryFunctionReleased(APlayerCharacter* user)
{
	//sets firing false so that on the next tick weapon does not check if it is possible to fire
	firing = false;

	particleSystem->Deactivate();
	audioComp->Stop();
}

void AWaspNestCudgel::SecondaryFunction(APlayerCharacter* user)
{

}

void AWaspNestCudgel::BeginPlay()
{
	Super::BeginPlay();
}

void AWaspNestCudgel::Reload(bool instantReload)
{
	// Play sound
	audioComp->SetSound(audioList[1]);
	audioComp->Play();

	Super::Reload(instantReload);
	reloading = true;
}

void AWaspNestCudgel::Tick(float DeltaSeconds)
{
	//should keep the beeswarm centered around the user, but doesn't
	Super::Tick(DeltaSeconds);
}

void AWaspNestCudgel::Fire()
{
	// radius of the effect
	float damageRadius = 500.0f;


	particleSystem->Activate();
	//should keep the beeswarm centered around the user, but doesn't
	//particleSystem->SetWorldLocation(controller->GetPawn()->GetActorLocation(), false, nullptr);

	// Goes through all AplyerCharacterActor and those that are in range take damage
	for (TActorIterator<APlayerCharacter> aItr(GetWorld()); aItr; ++aItr)
	{
		if (*aItr != Cast<APlayerCharacter>(controller->GetPawn()))
		{
			float distance = GetDistanceTo(*aItr);

			if (distance <= damageRadius)
			{
				UGameplayStatics::ApplyDamage(*aItr, damage, GetInstigatorController(), this, UDamageType::StaticClass());
				APlayerCharacter* tempChar = Cast<APlayerCharacter>(controller->GetPawn());
				aItr->TakeDamage(damage, DAMAGE_TYPE::NORMAL, Cast<class ACMPlayerController>(tempChar->GetController()));
			}
		}
	}

}
