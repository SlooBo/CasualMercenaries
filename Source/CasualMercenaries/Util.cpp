// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Util.h"
#include "PlayerCharacter.h"
#include "Projectile.h"

APlayerCharacter* Util::GetLocalPlayer(UWorld* world)
{
	return Cast<APlayerCharacter>(world->GetFirstPlayerController()->GetPawn());
}

APlayerCharacter* Util::GetPlayerCharacter(APlayerController* player)
{
	return Cast<APlayerCharacter>(player->GetPawn());
}

int32 Util::GetNumPlayers(UWorld* world)
{
	int32 numPlayers = 0;

	for (FConstPlayerControllerIterator iter = world->GetPlayerControllerIterator(); iter; ++iter)
		numPlayers++;

	return numPlayers;
}

float Util::GetDistanceToPlayer(const AActor* object, const APlayerCharacter* player)
{
	FVector objectLoc = object->GetActorLocation();
	FVector playerLoc = player->GetActorLocation();
	FVector diff = objectLoc - playerLoc;

	UCapsuleComponent* capsule = player->GetCapsuleComponent();
	if (capsule == NULL)
		return diff.Size();

	float radius = capsule->GetScaledCapsuleRadius();
	float halfHeight = capsule->GetScaledCapsuleHalfHeight();

	float objectRadius = 0.0f;
	if (Cast<AProjectile>(object) != NULL && Cast<AProjectile>(object)->CollisionComp != NULL)
		objectRadius = Cast<AProjectile>(object)->CollisionComp->GetScaledSphereRadius();

	diff = diff.GetAbs();

	// math behind this: we want to calculate the collision distance between two collision component shapes.
	// Actor's GetDistanceTo only measures the distance between actor locations, ignoring the collision components completely

	diff.X -= radius + objectRadius;
	diff.Y -= radius + objectRadius;
	diff.Z -= halfHeight + objectRadius;
	
	if (diff.X < 0.0f)
		diff.X = 0.0f;
	if (diff.Y < 0.0f)
		diff.Y = 0.0f;
	if (diff.Z < 0.0f)
		diff.Z = 0.0f;

	return diff.Size();
}