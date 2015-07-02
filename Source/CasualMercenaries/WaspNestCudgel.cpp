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
	weaponMesh->SetRelativeScale3D(FVector(0.5, 0.5, 0.25));


	//ParticleSystem
	particleSystem = FOI.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Wasps"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_PaskMyrsky.P_PaskMyrsky'"));
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
	//replication
	bReplicates = true;
}

void AWaspNestCudgel::PrimaryFunction(APlayerCharacter* user)
{
	firing = true;
	particleSystem->Activate();
}

void AWaspNestCudgel::PrimaryFunctionReleased(APlayerCharacter* user)
{
	firing = false;
	particleSystem->Deactivate();
}

void AWaspNestCudgel::SecondaryFunction(APlayerCharacter* user)
{

}

void AWaspNestCudgel::BeginPlay()
{
	Super::BeginPlay();
}

void AWaspNestCudgel::Reload()
{

}

void AWaspNestCudgel::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AWaspNestCudgel::Fire()
{
	float damageRadius = 500.0f;
	float damage = 5.0f;


	//UGameplayStatics::ApplyRadialDamage(GetWorld(), 25, this->GetActorLocation(), 200, UDamageType::DamageFalloff,
	//									this->GetOwner(), this->GetOwner(), Cast<APlayerCharacter>(this->GetOwner())->GetController(), );

	for (TActorIterator<APlayerCharacter> aItr(GetWorld()); aItr; ++aItr)
	{
		float distance = GetDistanceTo(*aItr);

		if (distance <= damageRadius)
		{
			UGameplayStatics::ApplyDamage(*aItr, damage, GetInstigatorController(), this, UDamageType::StaticClass());
			APlayerCharacter* tempChar = Cast<APlayerCharacter>(this->GetOwner());
			aItr->TakeDamage(damage, FDamageEvent::FDamageEvent(), Cast<APlayerController>(tempChar->GetController()), this);
		}
	}
}
