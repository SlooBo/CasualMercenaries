// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "PlayerCharacter.h"
#include "PomegranateLauncher.h"




APomegranateLauncher::APomegranateLauncher(const FObjectInitializer& FOI) : AWeapon(FOI)
{

}

void APomegranateLauncher::PrimaryFunction(APlayerCharacter* user)
{

}

void APomegranateLauncher::SecondaryFunction()
{
	
}

void APomegranateLauncher::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APomegranateLauncher::Tick(float DeltaSeconds)
{
	AWeapon::Tick(DeltaSeconds);

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

		// spawn the projectile at the muzzle
		AGrenade* const projectile = World->SpawnActor<AGrenade>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);

		if (projectile)
		{
			FVector const LaunchDir = MuzzleRotation.Vector();
			projectile->InitVelocity(LaunchDir);
		}
	}
}
