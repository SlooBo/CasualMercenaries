// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMPlayerController.h"
#include "PlayerCharacter.h"
#include "UnrealNetwork.h"
#include "CMGameMode.h"
#include "CMPlayerState.h"
#include "WeaponData.h"
#include "PlayerHud.h"


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
	musicCurrentTrack = -1;

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
	musicComponent->OnAudioFinished.RemoveDynamic(this, &ACMPlayerController::MusicPlay);
	musicComponent->Stop();

	Super::EndPlay(endPlayReason);
}

void ACMPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("AllChat", IE_Pressed, this, &ACMPlayerController::OpenAllChat);
	InputComponent->BindAction("TeamChat", IE_Pressed, this, &ACMPlayerController::OpenTeamChat);
	InputComponent->BindAction("OpenShop", IE_Pressed, this, &ACMPlayerController::OpenShop);

	InputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &ACMPlayerController::UseWeapon1);
	InputComponent->BindAction("LeftMouseButton", IE_Released, this, &ACMPlayerController::UseWeapon1Release);
	InputComponent->BindAction("RightMouseButton", IE_Pressed, this, &ACMPlayerController::UseWeapon2);
	InputComponent->BindAction("MouseWheelUp", IE_Pressed, this, &ACMPlayerController::SwitchWeaponUp);
	InputComponent->BindAction("MouseWheelDown", IE_Pressed, this, &ACMPlayerController::SwitchWeaponDown);
	InputComponent->BindAction("Reload", IE_Pressed, this, &ACMPlayerController::ReloadWeapon);

	InputComponent->BindAction("WeaponSlot1", IE_Pressed, this, &ACMPlayerController::WeaponSlot1);
	InputComponent->BindAction("WeaponSlot2", IE_Pressed, this, &ACMPlayerController::WeaponSlot2);
	InputComponent->BindAction("WeaponSlot3", IE_Pressed, this, &ACMPlayerController::WeaponSlot3);
	InputComponent->BindAction("WeaponSlot4", IE_Pressed, this, &ACMPlayerController::WeaponSlot4);

	InputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &ACMPlayerController::TryRespawn);
}

void ACMPlayerController::MusicPlay()
{
	musicCurrentTrack++;
	if (musicCurrentTrack >= musicList.Num())
		musicCurrentTrack = 0;

	if (musicCurrentTrack < musicList.Num())
	{
		musicComponent->SetSound(musicList[musicCurrentTrack]);
		musicComponent->Play();
	}
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

void ACMPlayerController::TryRespawn()
{
	if (IsAlive())
		return;

	RequestRespawn();
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

bool ACMPlayerController::IsAlive()
{
	ACMPlayerState *playerState = Cast<ACMPlayerState>(PlayerState);
	if (playerState != NULL)
		return playerState->IsAlive();
	return false;
}

void ACMPlayerController::OpenTeamChat()
{
	APlayerHud *playerhud = Cast<APlayerHud>(GetHUD());
}
void ACMPlayerController::OpenShop()
{
	APlayerHud *playerHud = Cast<APlayerHud>(GetHUD());

	if (playerHud->GetCurrentUI() == MenuType::SHOP)
		playerHud->changeUIElement(MenuType::GAME_UI);
	else
		playerHud->changeUIElement(MenuType::SHOP);
}
void ACMPlayerController::OpenAllChat()
{
	APlayerHud *playerHud = Cast<APlayerHud>(GetHUD());
}

/*
	Inventory
*/

void ACMPlayerController::ReloadWeapon()
{
	ServerReloadWeapon();
}

bool ACMPlayerController::ServerReloadWeapon_Validate()
{
	return true;
}

void ACMPlayerController::ServerReloadWeapon_Implementation()
{
	if (!IsAlive())
		return;

	if (inventory.GetCurrentWeapon() != nullptr)
		inventory.GetCurrentWeapon()->Reload();
}

void ACMPlayerController::AddWeapon(AWeapon* _weapon)
{
	inventory.AddWeaponToInventory(_weapon);
	//ServerAddWeapon(_weapon);
}

bool ACMPlayerController::ServerAddWeapon_Validate(AWeapon* _weapon)
{
	return true;
}

void ACMPlayerController::ServerAddWeapon_Implementation(AWeapon* _weapon)
{
	inventory.AddWeaponToInventory(_weapon);
}

void ACMPlayerController::UseWeapon1()
{
	ServerUseWeapon1();
}

bool ACMPlayerController::ServerUseWeapon1_Validate()
{
	return true;
}

void ACMPlayerController::ServerUseWeapon1_Implementation()
{
	if (!IsAlive())
		return;

	if (inventory.GetCurrentWeapon() != nullptr)
		inventory.GetCurrentWeapon()->PrimaryFunction(Cast<APlayerCharacter>(GetPawn()));
}

void ACMPlayerController::UseWeapon1Release()
{
	ServerUseWeapon1Release();
}

bool ACMPlayerController::ServerUseWeapon1Release_Validate()
{
	return true;
}

void ACMPlayerController::ServerUseWeapon1Release_Implementation()
{
	if (!IsAlive())
		return;

	if (inventory.GetCurrentWeapon() != nullptr)
		inventory.GetCurrentWeapon()->PrimaryFunctionReleased(Cast<APlayerCharacter>(GetPawn()));
}

void ACMPlayerController::UseWeapon2()
{
	//if (inventory.GetWeapon(currentWeapon) != nullptr)
	//	inventory.GetWeapon(currentWeapon)->SecondaryFunction(this);
	//ServerUseWeapon2();
}

bool ACMPlayerController::ServerUseWeapon2_Validate()
{
	return true;
}

void ACMPlayerController::ServerUseWeapon2_Implementation()
{
	if (!IsAlive())
		return;

	if (inventory.GetCurrentWeapon() != nullptr)
		inventory.GetCurrentWeapon()->SecondaryFunction(Cast<APlayerCharacter>(GetPawn()));
}

void ACMPlayerController::UseWeapon2Release()
{
	ServerUseWeapon1Release();
}

bool ACMPlayerController::ServerUseWeapon2Release_Validate()
{
	return true;
}

void ACMPlayerController::ServerUseWeapon2Release_Implementation()
{
	if (!IsAlive())
		return;

	if (inventory.GetCurrentWeapon() != nullptr)
		inventory.GetCurrentWeapon()->SecondaryFunctionReleased(Cast<APlayerCharacter>(GetPawn()));
}

void ACMPlayerController::SwitchWeaponUp()
{
	SwitchWeapon(inventory.currentWeapon + 1);
}
void ACMPlayerController::SwitchWeaponDown()
{
	SwitchWeapon(inventory.currentWeapon - 1);
}

void ACMPlayerController::WeaponSlot1()
{
	SwitchWeapon(0);
}
void ACMPlayerController::WeaponSlot2()
{
	SwitchWeapon(1);
}
void ACMPlayerController::WeaponSlot3()
{
	SwitchWeapon(2);
}
void ACMPlayerController::WeaponSlot4()
{
	SwitchWeapon(3);
}

void ACMPlayerController::SwitchWeapon(int newWeapon)
{
	if (!IsAlive())
		return;

	if (newWeapon >= inventory.weapons.Num())
		newWeapon = 0;
	else if (newWeapon < 0)
		newWeapon = inventory.weapons.Num() - 1;

	if (newWeapon != inventory.currentWeapon)
	{
		ServerSwitchWeapon(newWeapon, inventory.currentWeapon);
		inventory.currentWeapon = newWeapon;
	}
}

bool ACMPlayerController::ServerSwitchWeapon_Validate(float cw, float pw)
{
	return true;
}

void ACMPlayerController::ServerSwitchWeapon_Implementation(float newWeapon, float previousWeapon)
{
	inventory.currentWeapon = newWeapon;

	APlayerCharacter* character = Cast<APlayerCharacter>(GetPawn());

	if (inventory.GetWeapon(previousWeapon) != nullptr)
		inventory.GetWeapon(previousWeapon)->SetActorHiddenInGame(true);
	if (inventory.GetWeapon(newWeapon) != nullptr)
		inventory.GetWeapon(newWeapon)->SetActorHiddenInGame(false);
	if (inventory.GetWeapon(newWeapon) != nullptr && character != NULL)
		inventory.GetWeapon(newWeapon)->SetActorLocation(character->Mesh->GetSocketByName("GunSocket")->GetSocketLocation(character->Mesh));
}