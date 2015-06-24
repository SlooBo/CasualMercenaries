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


	user->GetActorEyesViewPoint(userLoc2, cameraRot);

	FTransform(cameraRot).TransformVector(MuzzleOffset);

	userLoc = user->GetActorLocation() + MuzzleOffset;
	
	const FVector shootDir = cameraRot.Vector();
	const FVector startTrace = userLoc;
	const FVector endTrace = startTrace + shootDir * 1000;

	FCollisionQueryParams traceParams(FName(TEXT("WeaponTrace")), true, this);
	traceParams.bTraceAsyncScene = true;
	traceParams.bReturnPhysicalMaterial = true;
	traceParams.AddIgnoredActor(user);

	FHitResult hit(ForceInit);

	GetWorld()->LineTraceSingle(hit, startTrace, endTrace, ECollisionChannel::ECC_Destructible, traceParams);

	DrawDebugLine(GetWorld(), startTrace, endTrace, FColor(260.0f, 0.0f, 0.f, 1.f), false, 1.f);

	APlayerCharacter*  player = Cast<APlayerCharacter>(hit.GetActor());
	player->TakeDamage(10, Cast<APlayerController>(user->GetController()));

	DrawLine(startTrace, endTrace);
	//oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi oijoi
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

void AMashineGun::SecondaryFunction()
{

}