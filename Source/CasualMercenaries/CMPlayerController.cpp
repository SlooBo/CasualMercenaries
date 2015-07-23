// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "CMPlayerController.h"
#include "PlayerCharacter.h"
#include "UnrealNetwork.h"
#include "CMGameMode.h"
#include "CMPlayerState.h"
#include "WeaponData.h"
#include "PlayerHud.h"
#include "ShopLogic.h"

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

	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("WeaponAudioComponent"));
	audioComp->SetVolumeMultiplier(0.125f);
	audioComp->bAutoActivate = false;
	audioComp->AttachParent = GetRootComponent();


	//TODO: add all music automatically if possible
	static ConstructorHelpers::FObjectFinder<USoundWave> Music1(TEXT("SoundWave'/Game/Game/Audio/Music/Artemisia_-_BiPilar.Artemisia_-_BiPilar'"));
	if (Music1.Object)
		musicList.AddUnique(Music1.Object);

	//loading of an audiosnippet
	static ConstructorHelpers::FObjectFinder<USoundWave> audio1(TEXT("SoundWave'/Game/Game/Audio/NotDash.NotDash'"));
	audioComp->SetSound(audio1.Object);
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

	InputComponent->BindAction("Reload", IE_Pressed, this, &ACMPlayerController::TryRespawn);
	InputComponent->BindAction("Escape_Back", IE_Pressed, this, &ACMPlayerController::OnPressedEscape);
	InputComponent->BindAction("ScoreSreen", IE_Pressed, this, &ACMPlayerController::PressedOpenScore);
	InputComponent->BindAction("ScoreSreen", IE_Released, this, &ACMPlayerController::PressedCloseScore);
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
	inventory.SetPlayer(this);
	if (inventory.weapons.Num() < 1)
	{
		for (int i = 0; i < 4; i++)
			inventory.AddWeaponToInventory(nullptr);
	}
}


void ACMPlayerController::OnPlayerDeath()
{
	if (inventory.GetCurrentWeapon() != NULL)
	{
		inventory.GetCurrentWeapon()->PrimaryFunctionReleased(Cast<APlayerCharacter>(this->GetPawn()));
		inventory.GetCurrentWeapon()->SetActorHiddenInGame(true);
	}

}

void ACMPlayerController::OnPlayerDeathBroadcast_Implementation(ACMPlayerController* killed, ACMPlayerController* killer/*, AWeapon* weapon*/)
{
	if (killed == this)
	{
		APlayerHud *hud = Cast<APlayerHud>(GetHUD());
		if (hud != nullptr)
		{
			hud->ShowText("After 5 seconds press R to respawn", 32, 0.5f, 0.2f, 10, FLinearColor::Red);
			hud->ShowText("Press B to open shop", 32, 0.5f, 0.7f, 10, FLinearColor::Red);
		}
		// handle clientside death here
	}

	// handle kill messages and other death related stuff here
}

void ACMPlayerController::OnAnnouncement_Implementation(const FString& announceText/*, USoundCue* announceSoundCue*/)
{
	APlayerHud *hud = Cast<APlayerHud>(GetHUD());
	if (hud != nullptr)
		hud->ShowText(announceText);
}


void ACMPlayerController::OnMatchStart_Implementation()
{

}

void ACMPlayerController::OnRoundStart_Implementation()
{
	APlayerHud* hud = Cast<APlayerHud>(GetHUD());
	if (hud != NULL)
		hud->CreateTestHavoc();
}

void ACMPlayerController::OnWarmupStart_Implementation()
{

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

void ACMPlayerController::Possess(APawn* inPawn)
{
	Super::Possess(inPawn);

	if (!HasAuthority())
		return;

	APlayerCharacter* pc = Cast<APlayerCharacter>(inPawn);
	AGhostCharacter* ghost = Cast<AGhostCharacter>(inPawn);
	if (pc != NULL && ghost == NULL)
	{
		// handle player character possession here
		for (int i = 0; i < 4; i++)
		{
			AWeapon* weapon = GetInventory().GetWeapon(i);
			if (weapon != NULL)
			{
				weapon->SetRoot(pc);
				weapon->SetOwner(pc);
			}
		}
		
	}
	else if (ghost != NULL)
	{
		// handle ghost character possession here
	}
	if (pc != NULL)
	{
		// handle both player and ghost character possession here
		if (rightShoulder)
			pc->SwitchShoulder();
	}
}
void ACMPlayerController::UnPossess()
{
	APlayerCharacter* pc = Cast<APlayerCharacter>(GetPawn());
	AGhostCharacter* ghost = Cast<AGhostCharacter>(GetPawn());
	if (pc != NULL && ghost == NULL)
	{
		// handle player character unpossession here
	}
	else if (ghost != NULL)
	{
		// handle ghost character unpossession here
	}
	if (pc != NULL)
	{
		// handle both player and ghost character unpossession here
		rightShoulder = pc->IsRightShoulder();
	}

	Super::UnPossess();
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

	if (currentWeapon != nullptr)
	{
		if (weaponid == currentWeapon->GetID())
			return;
	}
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
		if (previousWeaponStruct != nullptr)
		{
			playerState->AddMoney((int32)previousWeaponStruct->buyPrice / 2);
		}
		playerState->AddMoney(-(int32)weaponStruct->buyPrice);
	
		inventory.ChangeWeaponAtSlot(weaponIndex, weaponid);
		inventory.GetWeapon(weaponIndex)->SetActorHiddenInGame(false);

		//////////////////////////////Stuff////////////////////////////
		APlayerCharacter* character = Cast<APlayerCharacter>(GetPawn());
		AWeapon* newWeapon = inventory.GetWeapon(weaponIndex);

		if (newWeapon != nullptr)
		{
			USkeletalMeshComponent* mesh = character->GetMesh();

			//switch case caseroll for Miika to enjoy
			switch (newWeapon->GetID())
			{
			case WEAPONID::SHOT_GUN:
				newWeapon->weaponMesh->AttachTo(mesh, "ShotGunSocket", EAttachLocation::SnapToTargetIncludingScale);
				break;
			case WEAPONID::GRENADE_LAUNCHER:
				newWeapon->weaponMesh->AttachTo(mesh, "GrenadeLauncherSocket", EAttachLocation::SnapToTargetIncludingScale);
				break;
			case WEAPONID::MASHINE_GUN:
				newWeapon->weaponMesh->AttachTo(mesh, "MashineGunSocket", EAttachLocation::SnapToTargetIncludingScale);
				break;
			case WEAPONID::MUDBUSTER_GUN:
				newWeapon->weaponMesh->AttachTo(mesh, "MudBusterSocket", EAttachLocation::SnapToTargetIncludingScale);
				break;
			case WEAPONID::TWISTER_GUN:
				newWeapon->weaponMesh->AttachTo(mesh, "TwisterTorpedoGunSocket", EAttachLocation::SnapToTargetIncludingScale);
				break;
			case WEAPONID::ROCKET_LAUNCHER:
				newWeapon->weaponMesh->AttachTo(mesh, "RocketLauncherSocket", EAttachLocation::SnapToTargetIncludingScale);
				break;
			case WEAPONID::WASP_GUN:
				newWeapon->weaponMesh->AttachTo(mesh, "WaspGunSocket_R", EAttachLocation::SnapToTargetIncludingScale);
				break;
			case WEAPONID::WATER_GUN:
				newWeapon->weaponMesh->AttachTo(mesh, "WaterGunSocket", EAttachLocation::SnapToTargetIncludingScale);
				break;
			case WEAPONID::HOOK:
				newWeapon->weaponMesh->AttachTo(mesh, "ShotGunSocket", EAttachLocation::SnapToTargetIncludingScale);
				break;
			case WEAPONID::SMOKE_GUN:
				newWeapon->weaponMesh->AttachTo(mesh, "MashineGunSocket", EAttachLocation::SnapToTargetIncludingScale);
				break;
			default:
				gunSocket = mesh->GetSocketByName("GunSocket_null");
				break;
			}
		}
			////////////////////////EndStuff////////////////////////
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
	{
		APlayerCharacter *player = Cast<APlayerCharacter>(GetPawn());
		if (player != nullptr)
		player->RestoreActivity();
		playerHud->changeUIElement(MenuType::GAME_UI);
	}
	else
		playerHud->changeUIElement(MenuType::SHOP);
}

void ACMPlayerController::OpenAllChat()
{
	APlayerHud *playerHud = Cast<APlayerHud>(GetHUD());
}

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
	if(Cast<APlayerHud>(GetHUD())->GetCurrentUI() != MenuType::SHOP)
	SwitchWeapon(inventory.currentWeapon + 1);
	//TODO change shop weapons at shoplogic
}
void ACMPlayerController::SwitchWeaponDown()
{
	if (Cast<APlayerHud>(GetHUD())->GetCurrentUI() != MenuType::SHOP)
	SwitchWeapon(inventory.currentWeapon - 1);
	//TODO change shop weapons at shoplogic
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

void ACMPlayerController::SwitchWeapon(int32 newWeapon)
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

bool ACMPlayerController::ServerSwitchWeapon_Validate(int32 cw, int32 pw)
{
	return true;
}

void ACMPlayerController::ServerSwitchWeapon_Implementation(int32 cw, int32 pw)
{
	inventory.currentWeapon = cw;

	APlayerCharacter* character = Cast<APlayerCharacter>(GetPawn());

	if (character == nullptr)
		return;

	AWeapon* previousWeapon = inventory.GetWeapon(pw);
	AWeapon* newWeapon = inventory.GetWeapon(cw);

	if (previousWeapon != nullptr)
	{ 
		previousWeapon->SetActorHiddenInGame(true);
		previousWeapon->PrimaryFunctionReleased(character);
	}

	if (newWeapon != nullptr)
	{
		USkeletalMeshComponent* mesh = character->GetMesh();

		//switch case caseroll for Miika to enjoy
		switch (newWeapon->GetID())
		{
		case WEAPONID::SHOT_GUN:
			newWeapon->weaponMesh->AttachTo(mesh, "ShotGunSocket", EAttachLocation::SnapToTargetIncludingScale);
			break;
		case WEAPONID::GRENADE_LAUNCHER:
			newWeapon->weaponMesh->AttachTo(mesh, "GrenadeLauncherSocket", EAttachLocation::SnapToTargetIncludingScale);
			break;
		case WEAPONID::MASHINE_GUN:
			newWeapon->weaponMesh->AttachTo(mesh, "MashineGunSocket", EAttachLocation::SnapToTargetIncludingScale);
			break;
		case WEAPONID::MUDBUSTER_GUN:
			newWeapon->weaponMesh->AttachTo(mesh, "MudBusterSocket", EAttachLocation::SnapToTargetIncludingScale);
			break;
		case WEAPONID::TWISTER_GUN:
			newWeapon->weaponMesh->AttachTo(mesh, "TwisterTorpedoGunSocket", EAttachLocation::SnapToTargetIncludingScale);
			break;
		case WEAPONID::ROCKET_LAUNCHER:
			newWeapon->weaponMesh->AttachTo(mesh, "RocketLauncherSocket", EAttachLocation::SnapToTargetIncludingScale);
			break;
		case WEAPONID::WASP_GUN:
			newWeapon->weaponMesh->AttachTo(mesh, "WaspGunSocket_R", EAttachLocation::SnapToTargetIncludingScale);
			break;
		case WEAPONID::WATER_GUN:
			newWeapon->weaponMesh->AttachTo(mesh, "WaterGunSocket", EAttachLocation::SnapToTargetIncludingScale);
			break;
		case WEAPONID::HOOK:
			newWeapon->weaponMesh->AttachTo(mesh, "ShotGunSocket", EAttachLocation::SnapToTargetIncludingScale);
			break;
		case WEAPONID::SMOKE_GUN:
			newWeapon->weaponMesh->AttachTo(mesh, "MashineGunSocket", EAttachLocation::SnapToTargetIncludingScale);
			break;
		default:
			gunSocket = mesh->GetSocketByName("GunSocket_null");
			break;
		}

		if (gunSocket != nullptr)
		{
			//newWeapon->SetActorLocation(gunSocket->GetSocketLocation(character->GetMesh()));
			newWeapon->weaponMesh->AttachTo(mesh, "ShotgunSocket", EAttachLocation::SnapToTargetIncludingScale);
		}
		else
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "GunSocket not found pls fix");

		newWeapon->SetOwner(character);
		newWeapon->SetActorHiddenInGame(false);


		audioComp->Play();

	}

}
void ACMPlayerController::PrintTarget()
{
	ACMPlayerState *state = Cast<ACMPlayerState>(this->PlayerState);
	if (state != nullptr)
	{
		APawn *pawn = state->GetHuntTarget();
		if (pawn != nullptr)
		{
			if (pawn->PlayerState != nullptr)
			{
				FLinearColor color = Cast<ACMPlayerState>(pawn->PlayerState)->GetColorId();
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Hunt target: " + pawn->GetName());
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Hunt Color: " + color.ToString());
			}
			else
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Hunt target playerstate: null");
		}
		else
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Hunt target: null");
	}
}
void ACMPlayerController::OnPressedEscape()
{
	APlayerHud *hud = Cast<APlayerHud>(GetHUD());
	hud->changeUIElement(MenuType::PAUSE_MENU);
}
void ACMPlayerController::PressedOpenScore()
{
		APlayerHud *hud = Cast<APlayerHud>(GetHUD());
	hud->changeUIElement(MenuType::SCOREBOARD);
}
void ACMPlayerController::PressedCloseScore()
{
	APlayerHud *hud = Cast<APlayerHud>(GetHUD());
	hud->changeUIElement(MenuType::GAME_UI);
}