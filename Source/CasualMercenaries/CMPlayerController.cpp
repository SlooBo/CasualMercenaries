// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMPlayerController.h"
#include "PlayerCharacter.h"
#include "UnrealNetwork.h"
#include "MUDbuster.h"
#include "PomegranadeLauncher.h"
#include "WaspNestCudgel.h"
#include "MashineGun.h"

ACMPlayerController::ACMPlayerController(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	bReplicates = true;
}

void ACMPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACMPlayerController, inventory);
}

void ACMPlayerController::BeginPlay()
{
}

bool ACMPlayerController::ServerInitInventory_Validate()
{
	return true;
}
void ACMPlayerController::ServerInitInventory_Implementation()
{
	UWorld* const World = GetWorld();
	APlayerCharacter* playerPawn = Cast<APlayerCharacter>(GetPawn());

	FVector location = playerPawn->GetActorLocation();
	FRotator rotation = playerPawn->GetActorRotation();

	//inventory = NewObject<UInventory>();
	inventory.SetPlayer(playerPawn);
	inventory.ClearInventory();

	AMUDbuster* pyssy3 = World->SpawnActor<AMUDbuster>(AMUDbuster::StaticClass(), location, rotation);
	pyssy3->SetRoot(playerPawn);
	inventory.AddWeaponToInventory(pyssy3);

	APomeGranadeLauncher* pyssy4 = World->SpawnActor<APomeGranadeLauncher>(APomeGranadeLauncher::StaticClass(), location, rotation);
	pyssy4->SetRoot(playerPawn);
	inventory.AddWeaponToInventory(pyssy4);

	AWaspNestCudgel* pyssy2 = World->SpawnActor<AWaspNestCudgel>(AWaspNestCudgel::StaticClass(), location, rotation);
	pyssy2->SetRoot(playerPawn);
	inventory.AddWeaponToInventory(pyssy2);

	AMashineGun* pyssy = World->SpawnActor<AMashineGun>(AMashineGun::StaticClass(), location, rotation);
	pyssy->SetRoot(playerPawn);
	inventory.AddWeaponToInventory(pyssy);

	//inventory.GetWeapon(currentWeapon)->SetActorHiddenInGame(false);
	//inventoryInitialized = true;
}


void ACMPlayerController::OnPlayerDeath(ACMPlayerController* killed, ACMPlayerController* killer/*, AWeapon* weapon*/)
{
	if (killed == this)
	{
		APlayerCharacter* playerCharacter = static_cast<APlayerCharacter*>(GetPawn());
		if (playerCharacter != NULL)
		{
			UnPossess();
		}
	}
}