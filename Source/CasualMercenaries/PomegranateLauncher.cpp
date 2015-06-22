// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "PlayerCharacter.h"
#include "PomegranateLauncher.h"




APomegranateLauncher::APomegranateLauncher(const FObjectInitializer& FOI) : AWeapon(FOI)
{

}

void APomegranateLauncher::PrimaryFunction(APlayerCharacter* user)
{
	FVector userLoc;
	FRotator cameraRot;

	this->GetActorEyesViewPoint(userLoc, cameraRot);

	userLoc = this->GetActorLocation();

	FVector const MuzzleLocation = userLoc + FTransform(cameraRot).TransformVector(MuzzleOffset);

	FRotator MuzzleRotation = cameraRot;


	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		SpawnParams.bNoCollisionFail = true;

		// spawn the projectile at the muzzle
		AGranade* const projectile = World->SpawnActor<AGranade>(AGranade::StaticClass(), MuzzleLocation, MuzzleRotation, SpawnParams);
		volatile FVector loc = projectile->GetActorLocation();


		int asd = 0;
		if (projectile)
		{
			FVector const LaunchDir = MuzzleRotation.Vector();
			projectile->InitVelocity(LaunchDir);
		}
	}
}

void APomegranateLauncher::SecondaryFunction()
{
	
}

void APomegranateLauncher::BeginPlay()
{
	Super::BeginPlay();
	FVector userLoc;
	FRotator cameraRot;

	this->GetActorEyesViewPoint(userLoc, cameraRot);

	userLoc = this->GetActorLocation();

	FVector const MuzzleLocation = userLoc + FTransform(cameraRot).TransformVector(MuzzleOffset);

	FRotator MuzzleRotation = cameraRot;


	UWorld* const World = GetWorld();
	if (World != NULL)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		//SpawnParams.bNoCollisionFail = true;

		// spawn the projectile at the muzzle
		AGranade* const projectile = World->SpawnActor<AGranade>(AGranade::StaticClass(), MuzzleLocation, MuzzleRotation, SpawnParams);
		volatile FVector loc = projectile->GetActorLocation();


		int asd = 0;
		if (projectile)
		{
			FVector const LaunchDir = MuzzleRotation.Vector();
			projectile->InitVelocity(LaunchDir);
		}
	}
}

// Called every frame
void APomegranateLauncher::Tick(float DeltaSeconds)
{

}
