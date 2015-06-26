// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "Weapon.h"


// Sets default values
AWeapon::AWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

	weaponMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("RocketLauncherMesh"));
	//weaponMesh->SetOnlyOwnerSee(false);
	//weaponMesh->bCastDynamicShadow = true;
	//weaponMesh->CastShadow = true;
	//weaponMesh->SetSkeletalMesh(new USkeletalMesh(ObjectInitializer));

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->SetActorHiddenInGame(true);

	bReplicates = true;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AWeapon::PrimaryFunction(APlayerCharacter* user)
{


}

void AWeapon::SecondaryFunction()
{


}

void AWeapon::Reload()
{
	if (clips > 0)
	{
		ammo = ammoInClip;
		clips--;
	}
}