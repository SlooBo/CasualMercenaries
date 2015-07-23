// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "MUDbuster.h"
#include "MudBall.h"
#include "PlayerCharacter.h"




AMUDbuster::AMUDbuster(const FObjectInitializer& FOI) : Super(FOI)
{
	//skeletalMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/Weapons/MudBuster/Weapon/MESH_Mudbuster.MESH_Mudbuster'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);

	//skeletalMesh
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/MudBuster/Weapon/MAT_Mudbuster.MAT_Mudbuster'"));
	weaponMesh->SetMaterial(0, MateriaObj.Object);

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
	reloading = true;
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

	//one less bullet
	ammo--;

	FVector userLoc;
	FRotator cameraRot;

	this->GetOwner()->GetActorEyesViewPoint(userLoc, cameraRot);

	userLoc = weaponMesh->GetSocketLocation("ExhaustSocket");

	ACMPlayerController* asd = Cast<ACMPlayerController>(GetOwner());

	APlayerCharacter* pc = Cast<APlayerCharacter>(asd->GetPawn());

	FVector cameraLoc = pc->GetCamera()->GetComponentLocation();

	//muzzleoffset sets where the object spawns
	FVector const MuzzleLocation = userLoc + FTransform(cameraRot).TransformVector(muzzleOffset);

	FVector shootDir = cameraRot.Vector();


	//LineTrace
	const FVector startTrace = userLoc;
	const FVector endTrace = startTrace + shootDir * 20000;

	FCollisionQueryParams traceParams(FName(TEXT("WeaponTrace")), true, this);
	traceParams.bTraceAsyncScene = true;
	traceParams.bReturnPhysicalMaterial = true;
	traceParams.AddIgnoredActor(controller->GetPawn());

	FHitResult hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(hit, cameraLoc, endTrace, ECollisionChannel::ECC_Destructible, traceParams);


	FVector midle = hit.ImpactPoint;

	FVector tardines = (midle - startTrace);
	FVector sardines = startTrace + tardines;

	float asd1 = FVector::Dist(hit.Location, cameraLoc);
	float asd2 = FVector::Dist(userLoc, cameraLoc);
	if (asd1 < asd2)
	{
		traceParams.AddIgnoredActor(hit.GetActor());
		traceParams.AddIgnoredActor(hit.GetActor());
		GetWorld()->LineTraceSingleByChannel(hit, cameraLoc, endTrace, ECollisionChannel::ECC_Destructible, traceParams);
	}

	GetWorld()->LineTraceSingleByChannel(hit, startTrace, sardines, ECollisionChannel::ECC_Destructible, traceParams);


	//Play effect
	//ServerEffect(flavorParticleEffect, weaponMesh->GetSocketLocation("ExhaustSocket"));

	//get world
	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		//spawnparameters for projectile(rocket)
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = controller->GetPawn();
		SpawnParams.Instigator = Instigator;
		SpawnParams.bNoCollisionFail = true;


		// spawn the projectile at the muzzle
		AMudBall* const projectile = World->SpawnActor<AMudBall>(AMudBall::StaticClass(), MuzzleLocation, FRotator::ZeroRotator, SpawnParams);

		if (projectile)
		{
			//sets the owner(controller) of the projectile
			projectile->SetController(controller);

			//gives to the spawned projectile launch velocity

			projectile->InitVelocity(tardines.GetSafeNormal());

		}
	}
}