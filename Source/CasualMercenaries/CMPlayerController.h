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
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerInitInventory();

	UFUNCTION(Reliable, Server, WithValidation)
	void RequestRespawn();
	
	void OnPlayerDeath(ACMPlayerController* killed, ACMPlayerController* killer/*, AWeapon* weapon*/);
	void OnAnnouncement(FString announceText/*, USoundCue* announceSoundCue*/);
	void OnRoundStart();
	void OnWarmupStart();
	void OnShopAccessChanged(bool canShop);

	UFUNCTION()
	void MusicPlay();

	bool CanAccessShop() { return canShop; }
	FInventory& GetInventory() { return inventory; }

	UFUNCTION(Reliable, Server, WithValidation)
		void BuyWeapon(uint8 weaponIndex, WEAPONID weaponid);
	UFUNCTION(Exec)
	void AddMoney(int32 value);
protected:

	UPROPERTY(Replicated)
	bool canShop;

	UPROPERTY(Replicated)
	FInventory inventory;

	UPROPERTY()
	UAudioComponent* musicComponent;

	UPROPERTY()
	TArray<USoundWave*> musicList;
};