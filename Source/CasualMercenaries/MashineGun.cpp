// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "MashineGun.h"
#include "PlayerCharacter.h"//this should not be working...
#include "Projectile.h"
#include "CMPlayerController.h"

AMashineGun::AMashineGun(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	//skeletalMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/Weapons/ToasterGun/MESH_Toaster.MESH_Toaster'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);

	//material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/ToasterGun/MAT_toaster.MAT_toaster'"));
	weaponMesh->SetMaterial(0, MateriaObj.Object);

	//integer values
	maxAmmo = 120;
	clips = 999;
	ammo = 30;
	ammoInClip = 30;
	price = 600;

	//float values
	passedTimeReloading = 0;
	reloadTime = 0.5;
	firingInterval = .25;

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


	//particles
	particleSystem = FOI.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_MachineGun_Muzzle.P_MachineGun_Muzzle'"));
	particleSystem->Template = ParticleObj.Object;
	particleSystem->AttachTo(weaponMesh, "exhaustSocket");
	
	//Particles
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj2(TEXT("ParticleSystem'/Game/Game/Particles/P_MachineGun_Muzzle.P_MachineGun_Muzzle'"));
	flavorParticleEffect = ParticleObj2.Object;

	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj3(TEXT("ParticleSystem'/Game/Game/Particles/P_Bullet_Trail.P_Bullet_Trail'"));
	bulletTrail = ParticleObj3.Object;

	//ID
	id = WEAPONID::MASHINE_GUN;
	SuperFunctioAlaMiika();
	bReplicates = true;
}

void AMashineGun::BeginPlay()
{
	Super::BeginPlay();
}

void AMashineGun::Tick(float DeltaTime)
{
	AWeapon::Tick(DeltaTime);
}

void AMashineGun::Fire()
{
	FVector userLoc;
	FVector userLoc2;
	FRotator cameraRot;


	controller->GetPawn()->GetActorEyesViewPoint(userLoc2, cameraRot);

	FVector cameraLoc = Cast<APlayerCharacter>(GetOwner())->GetCamera()->GetComponentLocation();

	userLoc = controller->GetPawn()->GetActorLocation();


	FVector shootDir = cameraRot.Vector();


	//Bullet spread
	float random1 = (2 * FMath::FRand() - 1) * 0.05;
	float random2 = (2 * FMath::FRand() - 1) * 0.05;
	float random3 = (2 * FMath::FRand() - 1) * 0.05;
	shootDir.Set(shootDir.X + random1, shootDir.Y + random2, shootDir.Z + random3);



	//LineTrace
	const FVector startTrace = this->weaponMesh->GetSocketLocation("ExhaustSocket");
	const FVector endTrace = startTrace + shootDir * 20000;

	FCollisionQueryParams traceParams(FName(TEXT("WeaponTrace")), true, this);
	traceParams.bTraceAsyncScene = true;
	traceParams.bReturnPhysicalMaterial = true;
	traceParams.AddIgnoredActor(controller->GetPawn());

	FHitResult hit(ForceInit);


	GetWorld()->LineTraceSingleByChannel(hit, cameraLoc, endTrace, ECollisionChannel::ECC_Destructible, traceParams);

	FVector midle = hit.ImpactPoint;

	FVector tardines = (midle - startTrace) * 1.1;
	FVector sardines = startTrace + tardines;

	GetWorld()->LineTraceSingleByChannel(hit, startTrace, sardines, ECollisionChannel::ECC_Destructible, traceParams);


	//Play effect 
	ServerEffect(flavorParticleEffect, startTrace);


	//Hit resolve
	ABaseCharacter* player = Cast<ABaseCharacter>(hit.GetActor());
	if (player != nullptr)
		player->TakeDamage(damage, DAMAGE_TYPE::NORMAL, Cast<class ACMPlayerController>(Cast<class APlayerCharacter>(this->GetOwner())->GetController()));
	else
	{
		AProjectile* projectile = Cast<AProjectile>(hit.GetActor());
		if (projectile != nullptr)
			projectile->TakeDamage(damage);
	}

	muzzleOffset.X = 100;
	FVector const muzzleLocation = userLoc + FTransform(cameraRot).TransformVector(muzzleOffset);

	//Another effect
	DrawLine(startTrace, hit.ImpactPoint);
	ammo--;
}

void AMashineGun::PrimaryFunction(APlayerCharacter* user)
{
	this->SetOwner(user);
	if (ammo > 0)
	{
		firing = true;
	}
}

void AMashineGun::PrimaryFunctionReleased(APlayerCharacter* user)
{
	firing = false;
	particleSystem->Deactivate();
}

void AMashineGun::DrawLine(FVector begin, FVector end)
{	
	ServerDrawLine(begin, end);
}

bool AMashineGun::ServerDrawLine_Validate(FVector begin, FVector end)
{
	return true;
}

void AMashineGun::ServerDrawLine_Implementation(FVector begin, FVector end)
{
	audioComp->SetSound(audioList[0]);
	audioComp->Play();
	//DrawDebugLine(GetWorld(), begin, end, FColor(100.0f, 100.0f, 0.f, 1.f), false, 1.f);
	//bulletTrail->SetVectorParameter("BulletTrailEnd", end);
	UParticleSystemComponent *particlen = UGameplayStatics::SpawnEmitterAtLocation(this, bulletTrail, begin, FRotator::ZeroRotator, true);
	particlen->SetVectorParameter("BulletTrailEnd", end);
	particlen->SetActorParameter("BulletTrailBegin", this);
	particlen->Activate();
}

void AMashineGun::SecondaryFunction(APlayerCharacter* user)
{

}

void AMashineGun::Reload()
{
	if (clips > 0)
	{
		reloading = true;
		audioComp->SetSound(audioList[1]);
		audioComp->Play();
	}
}

bool AMashineGun::ServerEffect_Validate(UParticleSystem* particle, FVector location)
{
	return true;
}

void AMashineGun::ServerEffect_Implementation(UParticleSystem* particle, FVector location)
{
	UParticleSystemComponent *particlen = UGameplayStatics::SpawnEmitterAtLocation(this, particle, location, -this->GetActorRotation(), true);
	particlen->Activate();
	particleSystem->Activate();
}