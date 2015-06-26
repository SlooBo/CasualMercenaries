// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "PlayerCharacter.h"
#include "PomegranadeLauncher.h"




APomeGranadeLauncher::APomeGranadeLauncher(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/ToasterGun/MESH_Toaster_gun.MESH_Toaster_gun'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/ToasterGun/MAT_toaster.MAT_toaster'"));
	weaponMesh->SetMaterial(0, MateriaObj.Object);


	weaponMesh->SetRelativeScale3D(FVector(0.01, 0.05, 0.5));

	bReplicates = true;
}

void APomeGranadeLauncher::PrimaryFunction(APlayerCharacter* user)
{
	FVector userLoc;
	FRotator cameraRot;

	user->GetActorEyesViewPoint(userLoc, cameraRot);

	userLoc = user->GetActorLocation();

	FVector const MuzzleLocation = userLoc + FTransform(cameraRot).TransformVector(MuzzleOffset);
	MuzzleOffset.X = 100;
	FRotator MuzzleRotation = cameraRot;

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = user;
		SpawnParams.Instigator = Instigator;
		//SpawnParams.bNoCollisionFail = true;

		// spawn the projectile at the muzzle
		AGranade* const projectile = World->SpawnActor<AGranade>(AGranade::StaticClass(), MuzzleLocation, MuzzleRotation, SpawnParams);

		if (projectile)
		{
			FVector const LaunchDir = MuzzleRotation.Vector();
			projectile->InitVelocity(LaunchDir);
		}
	}
}

void APomeGranadeLauncher::SecondaryFunction()
{
	
}

void APomeGranadeLauncher::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APomeGranadeLauncher::Tick(float DeltaSeconds)
{

}
