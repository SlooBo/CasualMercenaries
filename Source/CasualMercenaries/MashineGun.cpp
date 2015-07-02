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

	

	//particles
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_MachineGun_Muzzle.P_MachineGun_Muzzle'"));
	flavorParticleEffect = ParticleObj.Object;

	//ID
	id = WEAPONID::MASHINE_GUN;
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
	if (ammo < 1)
	{
		firing = false;
		return;
	}
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
	APlayerCharacter* player = Cast<APlayerCharacter>(hit.GetActor());
	if (player != nullptr)
		player->TakeDamage(10, FDamageEvent::FDamageEvent(), Cast<class ACMPlayerController>(Cast<APlayerCharacter>(this->GetOwner())->GetController()), this);
	else
	{ 
		AProjectile* projectile = Cast<AProjectile>(hit.GetActor());
		if (projectile != nullptr)
			projectile->TakeDamage(20);// , FDamageEvent::FDamageEvent(), Cast<APlayerCharacter>(this->GetOwner())->GetController(), this);
	}


	//Another effect
	DrawLine(startTrace, endTrace);
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
	DrawDebugLine(GetWorld(), begin, end, FColor(100.0f, 100.0f, 0.f, 1.f), false, 1.f);
}

void AMashineGun::SecondaryFunction(APlayerCharacter* user)
{

}

void AMashineGun::Reload()
{
	if (clips > 0)
	{
		reloading = true;
	}
}

bool AMashineGun::ServerEffect_Validate(UParticleSystem* particle, FVector location)
{
	return true;
}

void AMashineGun::ServerEffect_Implementation(UParticleSystem* particle, FVector location)
{
	UParticleSystemComponent *particlen = UGameplayStatics::SpawnEmitterAtLocation(this, particle, location, FRotator::ZeroRotator, true);
}