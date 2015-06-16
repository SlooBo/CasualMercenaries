// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "MashineGun.h"
#include "PlayerCharacter.h"//this should not be working...

AMashineGun::AMashineGun(const FObjectInitializer& FOI) : AWeapon(FOI)
{
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
	FRotator cameraRot;

	userLoc = user->GetActorLocation();
	user->GetActorEyesViewPoint(userLoc, cameraRot);

	const FVector startTrace = userLoc;
	const FVector shootDir = cameraRot.Vector();
	const FVector endTrace = startTrace + shootDir * 200;

	FCollisionQueryParams traceParams(FName(TEXT("WeaponTrace")), true, this);
	traceParams.bTraceAsyncScene = true;
	traceParams.bReturnPhysicalMaterial = true;

	FHitResult hit(ForceInit);

	GetWorld()->LineTraceSingle(hit, startTrace, endTrace, ECollisionChannel::ECC_Destructible, traceParams);

	DrawDebugLine(GetWorld(), startTrace, endTrace, FColor(1.0f, 0.f, 0.f, 1.f), false, 10.f);

	//oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi
}

void AMashineGun::SecondaryFunction()
{

}