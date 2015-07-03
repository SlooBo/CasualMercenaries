// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "Inventory.h"
#include "CMPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API ACMPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACMPlayerController(const class FObjectInitializer& objectInitializer);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerInitInventory();

	UFUNCTION(Reliable, Server, WithValidation)
	void RequestRespawn();
	
	void OnPlayerDeath(ACMPlayerController* killed, ACMPlayerController* killer/*, AWeapon* weapon*/);

	FInventory& GetInventory() { return inventory; }

	UFUNCTION(Reliable, Server, WithValidation)
		void BuyWeapon(uint8 weaponIndex, WEAPONID weaponid);
protected:

	UPROPERTY(Replicated)
	FInventory inventory;
};