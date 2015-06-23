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
}

void AMashineGun::BeginPlay()
{
	Super::BeginPlay();

}

void AMashineGun::Tick(float DeltaTime)
{
	AWeapon::Tick(DeltaTime);
}

void AMashineGun::PrimaryFunction(APlayerCharacter* user)
{
	FVector userLoc;
	FVector userLoc2;
	FRotator cameraRot;

	userLoc = user->GetActorLocation();
	user->GetActorEyesViewPoint(userLoc2, cameraRot);

	const FVector startTrace = userLoc;
	const FVector shootDir = cameraRot.Vector();
	const FVector endTrace = startTrace - shootDir * 1000;

	FCollisionQueryParams traceParams(FName(TEXT("WeaponTrace")), true, this);
	traceParams.bTraceAsyncScene = true;
	traceParams.bReturnPhysicalMaterial = true;

	FHitResult hit(ForceInit);

	GetWorld()->LineTraceSingle(hit, startTrace, endTrace, ECollisionChannel::ECC_Destructible, traceParams);

	DrawDebugLine(GetWorld(), startTrace, endTrace, FColor(100.0f, 100.0f, 0.f, 1.f), false, 1.f);

	//oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi
}

void AMashineGun::SecondaryFunction()
{

}