// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "MashineGun.h"
#include "PlayerCharacter.h"//this should not be working...

AMashineGun::AMashineGun(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/ToasterGun/MESH_Toaster_gun.MESH_Toaster_gun'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/ToasterGun/MAT_toaster.MAT_toaster'"));
	weaponMesh->SetMaterial(0, MateriaObj.Object);


	weaponMesh->SetRelativeScale3D(FVector(0.05, 0.05, 0.05)); 

	bReplicates = true;

	reloadTime = 0.5;
	maxAmmo = 120;
	clips = 999;
	ammo = 30;
	ammoInClip = 30;
	firingInterval = .25;

	passedTimeReloading = 0;

	id = WEAPONID::MASHINE_GUN;
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

	float random1 = FMath::FRand() * 0.05;
	float random2 = FMath::FRand() * 0.05;
	float random3 = FMath::FRand() * 0.05;


	shootDir.Set(shootDir.X + random1, shootDir.Y + random2, shootDir.Z + random3);

	const FVector startTrace = userLoc;
	const FVector endTrace = startTrace + shootDir * 20000;

	FCollisionQueryParams traceParams(FName(TEXT("WeaponTrace")), true, this);
	traceParams.bTraceAsyncScene = true;
	traceParams.bReturnPhysicalMaterial = true;
	traceParams.AddIgnoredActor(this->GetOwner());

	FHitResult hit(ForceInit);

	GetWorld()->LineTraceSingle(hit, startTrace, endTrace, ECollisionChannel::ECC_Destructible, traceParams);

	//DrawDebugLine(GetWorld(), startTrace, endTrace, FColor(260.0f, 0.0f, 0.f, 1.f), false, 1.f);

	APlayerCharacter* player = Cast<APlayerCharacter>(hit.GetActor());
	if (player != nullptr)
		player->TakeDamage(10, Cast<APlayerController>(Cast<APlayerCharacter>(this->GetOwner())->GetController()));

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