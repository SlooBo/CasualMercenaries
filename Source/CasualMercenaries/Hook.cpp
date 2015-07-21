// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Hook.h"
#include "PlayerCharacter.h"
#include "math.h"
AHook::AHook(const FObjectInitializer& FOI) : AWeapon(FOI)
{
	//SkeletonMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/Weapons/RocketLauncher/RocketLauncher.RocketLauncher'"));
	weaponMesh->SetSkeletalMesh(MeshObj.Object);


	//Integers
	maxAmmo = 6;
	clips = 999;
	ammo = 1;
	ammoInClip = 1;


	//Floats
	firingInterval = 0.50;
	reloadTime = 1.5;
	id = WEAPONID::HOOK;
	SuperFunctioAlaMiika();
	bReplicates = true;

	cableComponent = FOI.CreateDefaultSubobject<UCableComponent>(this, "cable");
	const ConstructorHelpers::FObjectFinder<UMaterial> MateriaObj(TEXT("Material'/Game/Game/Materials/ParticleMaterials/M_Paper.M_Paper'"));
	cableComponent->SetMaterial(0, MateriaObj.Object);
	cableComponent->CableLength = 0;
	cableComponent->NumSegments = 1;
	currentState = HOOKSTATE::NOT_STARTED;
}

void AHook::BeginPlay()
{
	Super::BeginPlay();
	hooked = false;
}

// Called every frame
void AHook::Tick(float DeltaTime)
{
	AWeapon::Tick(DeltaTime);
	if (projectile == nullptr || !projectile->IsValidLowLevel())
	{
		return;
	}
	switch (currentState)
	{
		case HOOKSTATE::PROJECTILE_SHOT:
		{
			if (projectile->hit)
			{
				FlyTowardsProjectile();
				currentState = HOOKSTATE::GOING_TOWARDS_PROJECTILE;
			}
			break;

		}
		case HOOKSTATE::GOING_TOWARDS_PROJECTILE:
		{
			MoveTick();
			APlayerCharacter *player = Cast<APlayerCharacter>(GetOwner());
			if (FVector::Dist(player->GetActorLocation(), hookedLocation) < 1000.0f)
			{
				ReleaseHook();
			}
			break;
		}
	}







}

void AHook::PrimaryFunction(APlayerCharacter* user)
{
	this->SetOwner(user);
	if (ammo > 0 && !hooked && !shotProjectile)
	{
		firing = true;
	}
	else if (!hooked && !shotProjectile)
	{
		Reload();
	}
}
void AHook::PrimaryFunctionReleased(APlayerCharacter* user)
{
	firing = false;
}
void AHook::Reload()
{
	reloading = true;
}

void AHook::Fire()
{
	FVector userLoc;
	FVector userLoc2;
	FRotator cameraRot;


	this->GetOwner()->GetActorEyesViewPoint(userLoc2, cameraRot);

	FVector cameraLoc = Cast<APlayerCharacter>(GetOwner())->GetCamera()->GetComponentLocation();

	userLoc = this->GetOwner()->GetActorLocation();


	FVector shootDir = cameraRot.Vector();
	//LineTrace
	const FVector startTrace = userLoc;
	const FVector endTrace = startTrace + shootDir * 2000000;

	FCollisionQueryParams traceParams(FName(TEXT("WeaponTrace")), true, this);
	traceParams.bTraceAsyncScene = true;
	traceParams.bReturnPhysicalMaterial = true;
	traceParams.AddIgnoredActor(this->GetOwner());

	FHitResult hit(ForceInit);


	GetWorld()->LineTraceSingleByChannel(hit, cameraLoc, endTrace, ECollisionChannel::ECC_Destructible, traceParams);

	FVector midle = hit.ImpactPoint;

	FVector tardines = (midle - startTrace) * 1.1;
	FVector sardines = startTrace + tardines;

	GetWorld()->LineTraceSingleByChannel(hit, startTrace, sardines, ECollisionChannel::ECC_Destructible, traceParams);
	hookedLocation = hit.ImpactPoint;

	ammo--;
	//hooked = true;
	shotProjectile = true;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = controller->GetPawn();
	SpawnParams.Instigator = Instigator;

	AHookProjectile* const projectile = GetWorld()->SpawnActor<AHookProjectile>(AHookProjectile::StaticClass(), startTrace, LookAtRotator(startTrace,sardines), SpawnParams);
	this->projectile = projectile;
	if (projectile)
	{
		projectile->SetController(controller);
		FVector const LaunchDir = LookAtRotator(startTrace, sardines).Vector();
		projectile->InitVelocity(LaunchDir);
	}
	currentState = HOOKSTATE::PROJECTILE_SHOT;
	//FlyTowardsProjectile();
}
void AHook::ReleaseHook()
{
	currentState = HOOKSTATE::NOT_STARTED;
	hooked = false;
	shotProjectile = false;
	projectile->Destroy();
	cableComponent->EndLocation = FVector::ZeroVector;
	APlayerCharacter *player = Cast<APlayerCharacter>(GetOwner());
	if (player != nullptr)
	{
		player->GetCharacterMovement()->GravityScale = 1.0f;
		
	}

}
float AHook::EasedValue(float currentTime, float startValue, float changeInValue, float duration) 
{
	return changeInValue * (-pow(2, -10 * currentTime / duration) + 1) + startValue;
};
void AHook::FlyTowardsProjectile()
{
	GetWorld()->GetTimerManager().SetTimer(hookReleaseHandle, this, &AHook::ReleaseHook, 3.0f, false);
	APlayerCharacter *player = Cast<APlayerCharacter>(GetOwner());
	if (player != nullptr)
	{
		player->GetCharacterMovement()->GravityScale = 0.0f;
		player->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		startLocation = player->GetActorLocation();
		player->Jump();
	}
	if (projectile != nullptr && projectile->IsValidLowLevel())
	{
		FString name = "RocketMesh";
		cableComponent->SetAttachEndTo(projectile, FName(*name));
		cableComponent->Activate(true);
	}

}
FRotator AHook::LookAtRotator(FVector lookfrom, FVector lookat)
{
	FVector const worldUp(1.0f, 1.0f, 1.0f); // World Z Axis.
	return FRotationMatrix::MakeFromX(lookat - lookfrom).Rotator();
	
}
void AHook::MoveTick()
{
	APlayerCharacter *player = Cast<APlayerCharacter>(GetOwner());
	if (player != nullptr)
	{
		FVector forwardVector = projectile->GetActorLocation() - player->GetActorLocation();
		forwardVector.Normalize();
		float currentTime = GetWorld()->GetTimerManager().GetTimerElapsed(hookReleaseHandle);
		float easedvalue = EasedValue(currentTime, 0, 5000, 3.0f);
		player->GetCharacterMovement()->Velocity = forwardVector * easedvalue;


	}
}