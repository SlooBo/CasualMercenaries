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

	UPROPERTY()
		UAudioComponent* audioComp;

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

	virtual void Possess(APawn* inPawn) override;
	virtual void UnPossess() override;
	
	void OnPlayerDeath();

	UFUNCTION(Reliable, Client)
	void OnPlayerDeathBroadcast(ACMPlayerController* killed, ACMPlayerController* killer/*, AWeapon* weapon*/);

	UFUNCTION(Reliable, Client)
	void OnAnnouncement(const FString& announceText, int32 fontsize = 32, float anchorX = 0.5f, float anchorY = 0.5f, int32 lifetime = 2, FLinearColor color = FLinearColor::White/*, USoundCue* announceSoundCue*/);

	UFUNCTION(Reliable, Client)
	void OnMatchStart();

	UFUNCTION(Reliable, Client)
	void OnRoundStart();

	UFUNCTION(Reliable, Client)
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

	UFUNCTION(Reliable, Server, WithValidation)
	void AllowShooting(bool shootingAllowed);

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

	FVector GetMuzzleLocation(AWeapon* weapon);
	FVector GetWeaponAimDirection(AWeapon* weapon);

	void SwitchWeapon(int32 newWeapon);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSwitchWeapon(int32 cw, int32 pw);

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

	void PressedOpenScore();
	void PressedCloseScore();

	UFUNCTION(Reliable, Client)
	void UpdateMenuStatus();

protected:
	void OnPressedEscape();
	const USkeletalMeshSocket* gunSocket;

	bool rightShoulder;

	UPROPERTY(Replicated)
	bool canShop;

	UPROPERTY(Replicated)
	bool canShoot;

	UPROPERTY(Replicated)
	bool isShopping;

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

	UPROPERTY()
	USoundWave* audio;
};