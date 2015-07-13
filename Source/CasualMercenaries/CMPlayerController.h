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

	virtual void SetupInputComponent() override;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerInitInventory();

	void TryRespawn();

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

	bool IsAlive();

	/************************************************************************/
	/* Weapons                                                              */
	/************************************************************************/

	void AddWeapon(AWeapon* _weapon);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerAddWeapon(AWeapon* _weapon);

	void ReloadWeapon();
	void UseWeapon1();
	void UseWeapon1Release();
	void UseWeapon2();
	void UseWeapon2Release();

	void SwitchWeaponUp();
	void SwitchWeaponDown();
	void WeaponSlot1();
	void WeaponSlot2();
	void WeaponSlot3();
	void WeaponSlot4();

	void SwitchWeapon(int newWeapon);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSwitchWeapon(float cw, float cp);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerUseWeapon1();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerUseWeapon2();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerUseWeapon1Release();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerUseWeapon2Release();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerReloadWeapon();
	UFUNCTION(Exec)
	void PrintTarget();

protected:

	UPROPERTY(Replicated)
	bool canShop;

	UPROPERTY(Replicated)
	FInventory inventory;

	UPROPERTY()
	UAudioComponent* musicComponent;

	int32 musicCurrentTrack;

	UPROPERTY()
	TArray<USoundWave*> musicList;

	void OpenTeamChat();
	void OpenAllChat();
	void OpenShop();
};