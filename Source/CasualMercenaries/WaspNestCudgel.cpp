// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "WaspNestCudgel.h"
#include "PlayerCharacter.h"




AWaspNestCudgel::AWaspNestCudgel(const FObjectInitializer& FOI) : Super(FOI)
{
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/Weapons/Beehive/MESH_Beehive.MESH_Beehive'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);

	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/MudBuster/Weapon/MAT_Mudbuster.MAT_Mudbuster'")); // Material missing!!!!!
	weaponMesh->SetMaterial(0, MateriaObj.Object);

	weaponMesh->SetRelativeScale3D(FVector(0.5, 0.5, 0.25));


	particleSystem = FOI.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("Wasps"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_WaterSpray.P_WaterSpray'"));

	particleSystem->Template = ParticleObj.Object;

	particleSystem->AttachTo(weaponMesh, "WaspSocket");



	bReplicates = true;

	reloadTime = 2.5;
	maxAmmo = 120;
	clips = 999;
	ammo = 100;
	ammoInClip = 100;
	firingInterval = .5;

	passedTimeReloading = 0;

	id = WEAPONID::WASP_GUN;

	firing = false;
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
			aItr->TakeDamage(damage, Cast<APlayerController>(tempChar->GetController()));
		}
	}
}
