// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Shotgun.h"
#include "PlayerCharacter.h"
#include "Projectile.h"

AShotgun::AShotgun(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	//SkeletalMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/Weapons/Shotgun/MESH_Shotgun.MESH_Shotgun'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);


	//Material
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Weapons/Shotgun/MAT_Shotgun.MAT_Shotgun'"));
	weaponMesh->SetMaterial(0, MateriaObj.Object);


	//Integer values
	maxAmmo = 120;
	clips = 999;
	ammo = 6;
	ammoInClip = 6;
	price = 600;


	//Float values
	passedTimeReloading = 0;
	reloadTime = 2;
	firingInterval = .50;


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


	//Particles
	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj(TEXT("ParticleSystem'/Game/Game/Particles/P_MachineGun_Muzzle.P_MachineGun_Muzzle'"));
	flavorParticleEffect = ParticleObj.Object;


	const ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleObj2(TEXT("ParticleSystem'/Game/Game/Particles/P_Bullet_Trail.P_Bullet_Trail'"));
	bulletTrail = ParticleObj.Object;



	//ID
	id = WEAPONID::SHOT_GUN;


	SuperFunctioAlaMiika();

	//Replicates
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
	FVector userLoc;
	FVector userLoc2;
	FRotator cameraRot;



	controller->GetPawn()->GetActorEyesViewPoint(userLoc2, cameraRot);

	FVector cameraLoc = Cast<APlayerCharacter>(controller->GetPawn())->GetCamera()->GetComponentLocation();

	userLoc = controller->GetPawn()->GetActorLocation();

	FVector shootDir = cameraRot.Vector();
	for (int i = 0; i < 10; i++)
	{
		//Bullet spread
		float random1 = (2 * FMath::FRand() - 1) * 0.025;
		float random2 = (2 * FMath::FRand() - 1) * 0.025;
		float random3 = (2 * FMath::FRand() - 1) * 0.025;
		FVector random(random1, random2, random3);

		shootDir += random;

		//LineTrace
		const FVector startTrace = this->weaponMesh->GetSocketLocation("ExhaustSocket");
		const FVector endTrace = startTrace + shootDir * 20000;

		FCollisionQueryParams traceParams(FName(TEXT("WeaponTrace")), true, this);
		traceParams.bTraceAsyncScene = true;
		traceParams.bReturnPhysicalMaterial = true;
		traceParams.AddIgnoredActor(controller->GetPawn());

		FHitResult hit(ForceInit);


		GetWorld()->LineTraceSingleByChannel(hit, cameraLoc, endTrace, ECollisionChannel::ECC_Destructible, traceParams);

		GetWorld()->LineTraceSingleByChannel(hit, startTrace, hit.ImpactPoint, ECollisionChannel::ECC_Destructible, traceParams);


		//Play effect 
		ServerEffect(flavorParticleEffect, this->weaponMesh->GetSocketLocation("ExhaustSocket"));


		//Hit resolve
		ABaseCharacter* player = Cast<ABaseCharacter>(hit.GetActor());
		if (player != nullptr)
			player->TakeDamage(damage, DAMAGE_TYPE::NORMAL, Cast<class ACMPlayerController>(Cast<class APlayerCharacter>(controller->GetPawn())->GetController()));
		else
		{
			AProjectile* projectile = Cast<AProjectile>(hit.GetActor());
			if (projectile != nullptr)
				projectile->TakeDamage(damage);// , FDamageEvent::FDamageEvent(), Cast<APlayerCharacter>(this->GetOwner())->GetController(), this);
		}


		//Another effect
		DrawLine(startTrace, hit.ImpactPoint);
	
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
	audioComp->SetSound(audioList[0]);
	audioComp->Play();

	UParticleSystemComponent *particlen = UGameplayStatics::SpawnEmitterAtLocation(this, bulletTrail, begin, this->GetActorRotation(), true);
	particlen->SetVectorParameter("BulletTrailEnd", end);
	particlen->SetActorParameter("BulletTrailBegin", this);
}

void AShotgun::SecondaryFunction(APlayerCharacter* user)
{

}

void AShotgun::Reload()
{
	if (clips > 0)
	{
		reloading = true;
		audioComp->SetSound(audioList[1]);
		audioComp->Play();
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


