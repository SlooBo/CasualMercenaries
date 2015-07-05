// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMPlayerController.h"
#include "PlayerCharacter.h"
#include "UnrealNetwork.h"
#include "CMGameMode.h"

ACMPlayerController::ACMPlayerController(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	bReplicates = true;

	// setup music
	musicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicComponent"));
	musicComponent->SetVolumeMultiplier(0.125f);
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

	MusicPlay();
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

	//enable looping with timer
	FTimerHandle musicTimer;
	GetWorld()->GetTimerManager().SetTimer(musicTimer, this, &ACMPlayerController::MusicPlay, musicList[trackNumber]->GetDuration(), false);
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
		APlayerCharacter* playerCharacter = static_cast<APlayerCharacter*>(GetPawn());
		if (playerCharacter != NULL)
		{
			UnPossess();
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
	ACMGameMode* gameMode = static_cast<ACMGameMode*>(UGameplayStatics::GetGameMode(GetWorld()));
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
	inventory.ChangeWeaponAtSlot(weaponIndex, weaponid);
}