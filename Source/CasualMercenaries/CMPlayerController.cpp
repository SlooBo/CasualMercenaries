// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMPlayerController.h"
#include "PlayerCharacter.h"
#include "UnrealNetwork.h"
#include "CMGameMode.h"
#include "CMPlayerState.h"
#include "WeaponData.h"
ACMPlayerController::ACMPlayerController(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	bReplicates = true;

	//TODO: get music volume from options
	const float musicVolume = 0.125f;

	// setup music
	musicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicComponent"));
	musicComponent->OnAudioFinished.AddDynamic(this, &ACMPlayerController::MusicPlay);
	musicComponent->SetVolumeMultiplier(musicVolume);
	musicComponent->bIsMusic = true;
	musicComponent->bIsUISound = true;
	musicComponent->bAutoActivate = false;

	//TODO: add all music automatically if possible
	static ConstructorHelpers::FObjectFinder<USoundWave> Music1(TEXT("SoundWave'/Game/Game/Audio/Music/Artemisia_-_BiPilar.Artemisia_-_BiPilar'"));
	if (Music1.Object)
		musicList.AddUnique(Music1.Object);
}

void ACMPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACMPlayerController, inventory);
	DOREPLIFETIME(ACMPlayerController, canShop);
}

void ACMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only play music for clients
	if (GetNetMode() != NM_DedicatedServer)
 		MusicPlay();
}
void ACMPlayerController::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	Super::EndPlay(endPlayReason);

	musicComponent->OnAudioFinished.RemoveDynamic(this, &ACMPlayerController::MusicPlay);
	musicComponent->Stop();
}

void ACMPlayerController::MusicPlay()
{
	static int trackNumber = 0;

	if (trackNumber < musicList.Num())
	{
		musicComponent->SetSound(musicList[trackNumber]);
		musicComponent->Play();
	}

	trackNumber++;
	if (trackNumber >= musicList.Num())
		trackNumber = 0;
}

bool ACMPlayerController::ServerInitInventory_Validate()
{
	return true;
}
void ACMPlayerController::ServerInitInventory_Implementation()
{
	APlayerCharacter* playerPawn = Cast<APlayerCharacter>(GetPawn());
	inventory.SetPlayer(playerPawn);
	if (inventory.weapons.Num() < 1)
	{
		for (int i = 0; i < 4; i++)
			inventory.AddWeaponToInventory(nullptr);
	}
}


void ACMPlayerController::OnPlayerDeath(ACMPlayerController* killed, ACMPlayerController* killer/*, AWeapon* weapon*/)
{
	if (killed == this)
	{
		APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetPawn());
		if (playerCharacter != NULL)
		{
		}
	}
}

void ACMPlayerController::OnShopAccessChanged(bool canShop)
{
	this->canShop = canShop;
}

bool ACMPlayerController::RequestRespawn_Validate()
{
	return true;
}

void ACMPlayerController::RequestRespawn_Implementation()
{
	ACMGameMode* gameMode = Cast<ACMGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode != NULL)
	{
		if (gameMode->CanPlayerRespawn(this))
			gameMode->RespawnPlayer(this);
	}
}
bool ACMPlayerController::BuyWeapon_Validate(uint8 weaponIndex,WEAPONID weaponid)
{
	return true;
}
void ACMPlayerController::BuyWeapon_Implementation(uint8 weaponIndex, WEAPONID weaponid)
{
	//TODO check that player has money for it
	ACMPlayerState *playerState = Cast<ACMPlayerState>(PlayerState);
	AWeapon *currentWeapon = GetInventory().GetWeapon(weaponIndex);
	if (playerState == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Major error during buy!" );
		return;
	}
	FWeaponStruct *weaponStruct = WeaponData::Get()->GetWeaponData(weaponid);
	FWeaponStruct *previousWeaponStruct = nullptr;
	if (currentWeapon != nullptr)
		previousWeaponStruct = WeaponData::Get()->GetWeaponData(currentWeapon->GetID());
	if ((uint32)playerState->GetMoney() >= weaponStruct->buyPrice)
	{
		if (currentWeapon != nullptr)
		{
			playerState->AddMoney((int32)weaponStruct->buyPrice / 2);
		}
		playerState->AddMoney(-(int32)weaponStruct->buyPrice);
	
			inventory.ChangeWeaponAtSlot(weaponIndex, weaponid);
		
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Nut enuff cash!");
	}
}
void ACMPlayerController::AddMoney(int32 value)
{
	ACMPlayerState *playerState = Cast<ACMPlayerState>(PlayerState);
	playerState->AddMoney(value);
}