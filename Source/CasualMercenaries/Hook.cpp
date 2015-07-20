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

	cableComponent = FOI.CreateDefaultSubobject<UMyCable>(this, "cable");

	//cableComponent = FOI.CreateDefaultSubobject<UCableComponent>(this, TEXT("MyCable"));
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
	if (!hooked)
	{
		return;
	}
	APlayerCharacter *player = Cast<APlayerCharacter>(GetOwner());
	if (player != nullptr)
	{
		FVector forwardVector = hookedLocation - player->GetActorLocation();
		forwardVector.Normalize();
		float currentTime = GetWorld()->GetTimerManager().GetTimerElapsed(hookReleaseHandle);
		float test = EasedValue(currentTime, 0, 5000, 3.0f);
		player->GetCharacterMovement()->Velocity = forwardVector * test;


	}
	if (FVector::Dist(player->GetActorLocation(), hookedLocation) < 500.0f)
	{
		ReleaseHook();
	}
}

void AHook::PrimaryFunction(APlayerCharacter* user)
{
	this->SetOwner(user);
	if (ammo > 0 && !hooked)
	{
		firing = true;
	}
	else if (!hooked)
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
	hooked = true;
	GetWorld()->GetTimerManager().SetTimer(hookReleaseHandle, this, &AHook::ReleaseHook, 3.0f, false);
	APlayerCharacter *player = Cast<APlayerCharacter>(GetOwner());
	if (player != nullptr)
	{
		player->GetCharacterMovement()->GravityScale = 0.0f;
		player->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		startLocation = player->GetActorLocation();
		player->Jump();
	}
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, startLocation.ToString() + FString(",") + hookedLocation.ToString());
	cableComponent->EndLocation = hookedLocation;
}
void AHook::ReleaseHook()
{
	if (!hooked)
		return;
	hooked = false;
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
