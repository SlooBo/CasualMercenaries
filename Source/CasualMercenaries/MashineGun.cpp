// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "MashineGun.h"
#include "PlayerCharacter.h"//this should not be working...
#include "Projectile.h"

AMashineGun::AMashineGun(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	//skeletalMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/ToasterGun/MESH_Toaster_gun.MESH_Toaster_gun'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);

	//material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/ToasterGun/MAT_toaster.MAT_toaster'"));
	weaponMesh->SetMaterial(0, MateriaObj.Object);
	weaponMesh->SetRelativeScale3D(FVector(0.05, 0.05, 0.05)); 
	weaponMesh->SetRelativeRotation(FRotator(0, 140, 0));
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
					
	//particleSystem = FOI.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("MyParticle"));
	//const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj2(TEXT("ParticleSystem'/Game/Game/Particles/P_MachineGun_Muzzle.P_MachineGun_Muzzle'"));
	//particleSystem->Template = ParticleObj.Object;
	//particleSystem->AttachTo(weaponMesh, "exhaustSocket");

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

	this->GetOwner()->GetActorEyesViewPoint(userLoc2, cameraRot);
	userLoc = this->GetOwner()->GetActorLocation();

	FVector shootDir = cameraRot.Vector();

	//Bullet spread
	float random1 = FMath::FRand() * 0.05;
	float random2 = FMath::FRand() * 0.05;
	float random3 = FMath::FRand() * 0.05;
	shootDir.Set(shootDir.X + random1, shootDir.Y + random2, shootDir.Z + random3);

	//LineTrace
	const FVector startTrace = userLoc;
	const FVector endTrace = startTrace + shootDir * 20000;

	FCollisionQueryParams traceParams(FName(TEXT("WeaponTrace")), true, this);
	traceParams.bTraceAsyncScene = true;
	traceParams.bReturnPhysicalMaterial = true;
	traceParams.AddIgnoredActor(this->GetOwner());

	FHitResult hit(ForceInit);

	GetWorld()->LineTraceSingle(hit, startTrace, endTrace, ECollisionChannel::ECC_Destructible, traceParams);



	//Play effect 
	ServerEffect(flavorParticleEffect, startTrace);


	//Hit resolve
	ABaseCharacter* player = Cast<ABaseCharacter>(hit.GetActor());
	if (player != nullptr)
		player->TakeDamage(10, DAMAGE_TYPE::NORMAL, Cast<class ACMPlayerController>(Cast<class APlayerCharacter>(this->GetOwner())->GetController()));
	else
	{ 
		AProjectile* projectile = Cast<AProjectile>(hit.GetActor());
		if (projectile != nullptr)
			projectile->TakeDamage(20);// , FDamageEvent::FDamageEvent(), Cast<APlayerCharacter>(this->GetOwner())->GetController(), this);
	}

	muzzleOffset.X = 100;
	FVector const muzzleLocation = userLoc + FTransform(cameraRot).TransformVector(muzzleOffset);

	//Another effect
	DrawLine(muzzleLocation, endTrace);
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
	int i =+ 2;
	audioComp->SetSound(audioList[0]);
	audioComp->Play();
	//DrawDebugLine(GetWorld(), begin, end, FColor(100.0f, 100.0f, 0.f, 1.f), false, 1.f);
	//bulletTrail->SetVectorParameter("BulletTrailEnd", end);
	UParticleSystemComponent *particlen = UGameplayStatics::SpawnEmitterAtLocation(this, bulletTrail, begin, FRotator::ZeroRotator, true);
	particlen->SetVectorParameter("BulletTrailEnd", end);
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
		audioComp->SetVolumeMultiplier(2);
		audioComp->Play();
	}
}

bool AMashineGun::ServerEffect_Validate(UParticleSystem* particle, FVector location)
{
	return true;
}

void AMashineGun::ServerEffect_Implementation(UParticleSystem* particle, FVector location)
{
	UParticleSystemComponent *particlen = UGameplayStatics::SpawnEmitterAtLocation(this, particle, location, FRotator::ZeroRotator, true);
	particleSystem->Activate();
}