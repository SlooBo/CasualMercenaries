// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "PlayerCharacter.h"
#include "PomegranadeLauncher.h"




APomeGranadeLauncher::APomeGranadeLauncher(const FObjectInitializer& FOI) : AWeapon(FOI)
{

}

void APomeGranadeLauncher::PrimaryFunction(APlayerCharacter* user)
{
	FVector userLoc;
	FRotator cameraRot;

	user->GetActorEyesViewPoint(userLoc, cameraRot);

	userLoc = user->GetActorLocation();

	FVector const MuzzleLocation = userLoc + FTransform(cameraRot).TransformVector(MuzzleOffset);

	FRotator MuzzleRotation = cameraRot;
	//MuzzleRotation.Pitch += 10;

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		//SpawnParams.bNoCollisionFail = true;

		// spawn the projectile at the muzzle
		AGranade* const projectile = World->SpawnActor<AGranade>(AGranade::StaticClass(), MuzzleLocation, MuzzleRotation, SpawnParams);

		if (projectile)
		{
			FVector const LaunchDir = MuzzleRotation.Vector();
			projectile->InitVelocity(LaunchDir * 5);
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
