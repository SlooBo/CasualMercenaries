// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "RocketLauncher.h"
#include "PlayerCharacter.h"//this should not be working...
ARocketLauncher::ARocketLauncher(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	//SkeletonMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/Weapons/RocketLauncher/RocketLauncher.RocketLauncher'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);


	//Integers
	maxAmmo = 6;
	clips = 999;
	ammo = 6;
	ammoInClip = 6;


	//Floats
	firingInterval = 0.50;
	reloadTime = 1.5;

	//Audio
	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	audioComp->SetVolumeMultiplier(0.125f);
	audioComp->bAutoActivate = false;
	audioComp->AttachParent = GetRootComponent();

	static ConstructorHelpers::FObjectFinder<USoundWave> audio1(TEXT("SoundWave'/Game/Game/Audio/RocketLauncher_Shoot.RocketLauncher_Shoot'"));
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
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj2(TEXT("ParticleSystem'/Game/Game/Particles/P_RocketLauncher_MuzzleBack.P_RocketLauncher_MuzzleBack'"));
	flavorParticleEffect = ParticleObj2.Object;


	//ID
	id = WEAPONID::ROCKET_LAUNCHER;
	SuperFunctioAlaMiika();

	//replication
	bReplicates = true;
}

void ARocketLauncher::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARocketLauncher::Tick(float DeltaTime)
{
	AWeapon::Tick(DeltaTime);
}

void ARocketLauncher::PrimaryFunction(APlayerCharacter* user)
{
	this->SetOwner(user);
	if (ammo > 0)
	{
		audioComp->SetSound(audioList[0]);
		audioComp->Play();
		firing = true;
	}
	else
	{	
		Reload();
	}
}

void ARocketLauncher::PrimaryFunctionReleased(APlayerCharacter* user)
{
	firing = false;
}
void ARocketLauncher::Reload()
{
	reloading = true;
}

void ARocketLauncher::Fire()
{
	//one less bullet
	ammo--;
	
	FVector userLoc;
	FRotator cameraRot;

	this->GetOwner()->GetActorEyesViewPoint(userLoc, cameraRot);

	userLoc = weaponMesh->GetSocketLocation("ExhaustSocket");// controller->GetPawn()->GetActorLocation();

	FVector cameraLoc = Cast<APlayerCharacter>(GetOwner())->GetCamera()->GetComponentLocation();

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
	ServerEffect(flavorParticleEffect, weaponMesh->GetSocketLocation("ExhaustSocket"));

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
		ARocket* const projectile = World->SpawnActor<ARocket>(ARocket::StaticClass(), MuzzleLocation, FRotator::ZeroRotator, SpawnParams);

		if (projectile)
		{
			//sets the owner(controller) of the projectile
			projectile->SetController(controller);

			//gives to the spawned projectile launch velocity

			projectile->InitVelocity(tardines.GetSafeNormal());

		}
	}
}

void ARocketLauncher::SecondaryFunction(APlayerCharacter* user)
{

}

bool ARocketLauncher::ServerEffect_Validate(UParticleSystem* particle, FVector location)
{
	return true;
}

void ARocketLauncher::ServerEffect_Implementation(UParticleSystem* particle, FVector location)
{
	UParticleSystemComponent *particlen = UGameplayStatics::SpawnEmitterAtLocation(this, particle, location, -this->GetActorRotation(), true);
	audioComp->SetSound(audioList[0]);
	audioComp->Play();
}