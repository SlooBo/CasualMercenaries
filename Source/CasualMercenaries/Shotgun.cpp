// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Shotgun.h"
#include "PlayerCharacter.h"
#include "Projectile.h"

AShotgun::AShotgun(const FObjectInitializer& FOI) : AWeapon(FOI)
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
	ammo = 6;
	ammoInClip = 6;
	price = 600;

	//float values
	passedTimeReloading = 0;
	reloadTime = 2;
	firingInterval = .50;



	//particles
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_MachineGun_Muzzle.P_MachineGun_Muzzle'"));
	flavorParticleEffect = ParticleObj.Object;


	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj2(TEXT("ParticleSystem'/Game/Game/Particles/P_BulletTrail.P_BulletTrail'"));
	bulletTrail = ParticleObj.Object;

	//ID
	id = WEAPONID::SHOT_GUN;
	bReplicates = true;
}

void AShotgun::BeginPlay()
{
	Super::BeginPlay();
}

void AShotgun::Tick(float DeltaTime)
{
	AWeapon::Tick(DeltaTime);
}

void AShotgun::Fire()
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
	for (int i = 0; i < 10; i++)
	{
		//Bullet spread
		float random1 = FMath::FRand() * 0.1;
		float random2 = FMath::FRand() * 0.1;
		float random3 = FMath::FRand() * 0.1;
		FVector random(random1, random2, random3);
		shootDir.Set(shootDir.X, shootDir.Y, shootDir.Z);

		//LineTrace
		const FVector startTrace = userLoc;
		const FVector endTrace = startTrace + (shootDir + random) * 20000;

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
			player->TakeDamage(10, DAMAGE_TYPE::NORMAL, Cast<class APlayerController>(Cast<class APlayerCharacter>(this->GetOwner())->GetController()));
		else
		{
			AProjectile* projectile = Cast<AProjectile>(hit.GetActor());
			if (projectile != nullptr)
				projectile->TakeDamage(20);// , FDamageEvent::FDamageEvent(), Cast<APlayerCharacter>(this->GetOwner())->GetController(), this);
		}


		//Another effect
		DrawLine(startTrace, endTrace);
	
	}
	 ammo--;
}

void AShotgun::PrimaryFunction(APlayerCharacter* user)
{
	this->SetOwner(user);
	if (ammo > 0)
	{
		firing = true;
	}
}

void AShotgun::PrimaryFunctionReleased(APlayerCharacter* user)
{
	firing = false;
}

void AShotgun::DrawLine(FVector begin, FVector end)
{
	ServerDrawLine(begin, end);
}

bool AShotgun::ServerDrawLine_Validate(FVector begin, FVector end)
{
	return true;
}

void AShotgun::ServerDrawLine_Implementation(FVector begin, FVector end)
{
	DrawDebugLine(GetWorld(), begin, end, FColor(100.0f, 100.0f, 0.f, 1.f), false, 1.f);
	//bulletTrail->SetVectorParameter("BulletTrailEnd", end);
	//UParticleSystemComponent *particlen = UGameplayStatics::SpawnEmitterAtLocation(this, bulletTrail, begin, FRotator::ZeroRotator, true);
	//particlen->SetVectorParameter("BulletTrailEnd", end);
}

void AShotgun::SecondaryFunction(APlayerCharacter* user)
{

}

void AShotgun::Reload()
{
	if (clips > 0)
	{
		reloading = true;
	}
}

bool AShotgun::ServerEffect_Validate(UParticleSystem* particle, FVector location)
{
	return true;
}

void AShotgun::ServerEffect_Implementation(UParticleSystem* particle, FVector location)
{
	UParticleSystemComponent *particlen = UGameplayStatics::SpawnEmitterAtLocation(this, particle, location, FRotator::ZeroRotator, true);
}


