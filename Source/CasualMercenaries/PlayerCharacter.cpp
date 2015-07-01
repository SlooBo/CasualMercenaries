// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "UnrealNetwork.h"
#include "PlayerCharacter.h"
#include "PlayerHud.h"
#include "CMGameMode.h"
#include "UberWeihmacher.h"
#include "MashineGun.h"
#include "RocketLauncher.h"
#include "PomegranadeLauncher.h"
#include "MUDbuster.h"
#include "WaspNestCudgel.h"

// Sets default values
APlayerCharacter::APlayerCharacter(const class FObjectInitializer& ObjectInitializer)
{
	inventory = NewObject<UInventory>();
	inventoryInitialized = false;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UCharacterMovementComponent* tempMoveComp = GetCharacterMovement();
	//Addjusting jump and movement
	tempMoveComp->GravityScale = 1.0;
	tempMoveComp->JumpZVelocity = 600;
	tempMoveComp->AirControl = 0.8f;
	tempMoveComp->MaxWalkSpeed = 1500;
	springArmComp = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraSpring"));
	//these 2 need finetuning
	springArmComp->SocketOffset = FVector(0, -100, 20);
	springArmComp->TargetOffset = FVector(0, 0, 55);
	springArmComp->bUsePawnControlRotation = true;
	springArmComp->TargetArmLength = 800;
	springArmComp->AttachParent = GetRootComponent();

	cameraComp = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
	cameraComp->AttachParent = springArmComp;

	//Audio component initialization
	audioComp = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComp"));
	audioComp->bAutoActivate = false;
	audioComp->bAutoDestroy = false;
	audioComp->AttachParent = GetRootComponent();

	rightShoulder = false;

	health_Max = 100.0f;
	health = health_Max;

	stamina_Max = 100.0f;
	stamina = stamina_Max;

	armor_Max = 100.0f;
	armor = armor_Max;

	wallTouch = false;
	dash_Multiplier = 0;

	currentWeapon = 0;

	fuckthisshit = FVector();

	state = CHARACTER_STATE::ALIVE;
	rounds = 0;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlayCplusplus()
{
	ServerInitInventory();
}

bool APlayerCharacter::ServerInitInventory_Validate()
{
	return true;
}
void APlayerCharacter::ServerInitInventory_Implementation()
{
	UWorld* const World = GetWorld();
	inventory = NewObject<UInventory>();
	inventory->SetPlayer(this);
	inventory->ClearInventory();

	AMUDbuster* pyssy3 = World->SpawnActor<AMUDbuster>(AMUDbuster::StaticClass(), this->GetActorLocation(), this->GetActorRotation());
	pyssy3->SetRoot(this);
	AddWeapon(pyssy3);

	APomeGranadeLauncher* pyssy4 = World->SpawnActor<APomeGranadeLauncher>(APomeGranadeLauncher::StaticClass(), this->GetActorLocation(), this->GetActorRotation());
	pyssy4->SetRoot(this);
	AddWeapon(pyssy4);

	AWaspNestCudgel* pyssy2 = World->SpawnActor<AWaspNestCudgel>(AWaspNestCudgel::StaticClass(), this->GetActorLocation(), this->GetActorRotation());
	pyssy2->SetRoot(this);
	AddWeapon(pyssy2);

	AMashineGun* pyssy = World->SpawnActor<AMashineGun>(AMashineGun::StaticClass(), this->GetActorLocation(), this->GetActorRotation());
	pyssy->SetRoot(this);
	AddWeapon(pyssy);

	//inventory->GetWeapon(currentWeapon)->SetActorHiddenInGame(false);
	inventoryInitialized = true;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	WallCheck();
	CheckStatus();

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	//Movement
	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	InputComponent->BindAction("LeftShift", IE_Pressed, this, &APlayerCharacter::InputDash);

	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::OnStopJump);

	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::WallJump);

	InputComponent->BindAction("SwitchShoulder", IE_Pressed, this, &APlayerCharacter::SwitchShoulder);

	InputComponent->BindAction("AllChat", IE_Pressed, this, &APlayerCharacter::OpenAllChat);
	InputComponent->BindAction("TeamChat", IE_Pressed, this, &APlayerCharacter::OpenTeamChat);

	InputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &APlayerCharacter::UseWeapon1);
	InputComponent->BindAction("LeftMouseButton", IE_Released, this, &APlayerCharacter::UseWeapon1Release);
	InputComponent->BindAction("RightMouseButton", IE_Pressed, this, &APlayerCharacter::UseWeapon2);
	InputComponent->BindAction("MouseWheelUp", IE_Pressed, this, &APlayerCharacter::SwitchWeaponUp);
	InputComponent->BindAction("MouseWheelDown", IE_Pressed, this, &APlayerCharacter::SwitchWeaponDown);
	InputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::ReloadWeapon);

	InputComponent->BindAction("WeaponSlot1", IE_Pressed, this, &APlayerCharacter::WeaponSlot1);
	InputComponent->BindAction("WeaponSlot2", IE_Pressed, this, &APlayerCharacter::WeaponSlot2);
	InputComponent->BindAction("WeaponSlot3", IE_Pressed, this, &APlayerCharacter::WeaponSlot3);
	InputComponent->BindAction("WeaponSlot4", IE_Pressed, this, &APlayerCharacter::WeaponSlot4);
}

void APlayerCharacter::ReloadWeapon()
{

	ServerReloadWeapon();
}

bool APlayerCharacter::ServerReloadWeapon_Validate()
{
	return true;
}

void APlayerCharacter::ServerReloadWeapon_Implementation()
{
	if (inventory->GetWeapon(currentWeapon) != nullptr)
		inventory->GetWeapon(currentWeapon)->Reload();
}

void APlayerCharacter::AddWeapon(AWeapon* _weapon)
{
	inventory->AddWeaponToInventory(_weapon);
	//ServerAddWeapon(_weapon);
}

bool APlayerCharacter::ServerAddWeapon_Validate(AWeapon* _weapon)
{
	return true;
}

void APlayerCharacter::ServerAddWeapon_Implementation(AWeapon* _weapon)
{
	inventory->AddWeaponToInventory(_weapon);
}

void APlayerCharacter::UseWeapon1()
{
	//if (inventory.GetWeapon(currentWeapon) != nullptr)
		ServerUseWeapon1();
}

bool APlayerCharacter::ServerUseWeapon1_Validate()
{
	return true;
}

void APlayerCharacter::ServerUseWeapon1_Implementation()
{
	if (inventory == nullptr)
		return;

	if (inventory->GetWeapon(currentWeapon) != nullptr)
		inventory->GetWeapon(currentWeapon)->PrimaryFunction(this);
}

void APlayerCharacter::UseWeapon1Release()
{
	ServerUseWeapon1Release();
}

bool APlayerCharacter::ServerUseWeapon1Release_Validate()
{
	return true;
}

void APlayerCharacter::ServerUseWeapon1Release_Implementation()
{
	if (inventory->GetWeapon(currentWeapon) != nullptr)
		inventory->GetWeapon(currentWeapon)->PrimaryFunctionReleased(this);
}

void APlayerCharacter::UseWeapon2()
{
	//if (inventory->GetWeapon(currentWeapon) != nullptr)
	//	inventory->GetWeapon(currentWeapon)->SecondaryFunction(this);
	//ServerUseWeapon2();
}

bool APlayerCharacter::ServerUseWeapon2_Validate()
{
	return true;
}

void APlayerCharacter::ServerUseWeapon2_Implementation()
{
	if (inventory == nullptr)
		return;

	if (inventory->GetWeapon(currentWeapon) != nullptr)
		inventory->GetWeapon(currentWeapon)->SecondaryFunction(this);
}

void APlayerCharacter::UseWeapon2Release()
{
	ServerUseWeapon1Release();
}

bool APlayerCharacter::ServerUseWeapon2Release_Validate()
{
	return true;
}

void APlayerCharacter::ServerUseWeapon2Release_Implementation()
{
	if (inventory->GetWeapon(currentWeapon) != nullptr)
		inventory->GetWeapon(currentWeapon)->SecondaryFunctionReleased(this);
}

void APlayerCharacter::SwitchWeaponUp()
{	
	int tempLastWeapon = currentWeapon;
	currentWeapon++;
	if (currentWeapon > 3)
		currentWeapon = 0;
	ServerSwitchWeapon(currentWeapon, tempLastWeapon);
}

void APlayerCharacter::SwitchWeaponDown()
{
	int tempLastWeapon = currentWeapon;
	currentWeapon--;
	if (currentWeapon < 0)
		currentWeapon = 3;
	ServerSwitchWeapon(currentWeapon, tempLastWeapon);
}


bool APlayerCharacter::ServerSwitchWeapon_Validate(float cw, float pw)
{
	return true;
}

void APlayerCharacter::ServerSwitchWeapon_Implementation(float cw, float pw)
{
	currentWeapon = cw;

	if (inventory != nullptr)
	{
		if (inventory->GetWeapon(pw) != nullptr)
			inventory->GetWeapon(pw)->SetActorHiddenInGame(true);
		if (inventory->GetWeapon(cw) != nullptr)
			inventory->GetWeapon(cw)->SetActorHiddenInGame(false);
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Value is already updated locally, skip in replication step
	//DOREPLIFETIME_CONDITION(APlayerCharacter, value, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APlayerCharacter, bIsJumping, COND_SkipOwner);

	//Replicated to every client, no special condition required
	//DOREPLIFETIME(APlayerCharacter, value);
	DOREPLIFETIME(APlayerCharacter, health);
	DOREPLIFETIME(APlayerCharacter, stamina);
	DOREPLIFETIME(APlayerCharacter, armor);
	DOREPLIFETIME(APlayerCharacter, state);
	
};

void APlayerCharacter::CheckStatus()
{
	if (state == CHARACTER_STATE::STUNNED)
	{
		rounds++;
		if (rounds > 500)
			state = CHARACTER_STATE::ALIVE;
	}
}

void APlayerCharacter::OnStartJump()
{
	if (IsMoveInputIgnored())
		return;

	bPressedJump = true;

	
}

void APlayerCharacter::OnStopJump()
{

}

float APlayerCharacter::TakeDamage(float _damage, struct FDamageEvent const& _damageEvent, class AController* _eventInstigator, class AActor* _damageCauser)
{
	return 4.0f;
}

void APlayerCharacter::OnDeath_Implementation(ACMPlayerController* killer)
{
	ServerOnDeath(killer);
}

bool APlayerCharacter::ServerOnDeath_Validate(ACMPlayerController* killer)
{
	return true;
}

void APlayerCharacter::ServerOnDeath_Implementation(ACMPlayerController* killer)
{
	inventory->ClearInventory();

	ACMGameMode* gameMode = static_cast<ACMGameMode*>(UGameplayStatics::GetGameMode(GetWorld()));
	ACMPlayerController* playerController = static_cast<ACMPlayerController*>(GetController());

	if (gameMode != NULL && playerController != NULL)
		gameMode->OnPlayerDeath(playerController, killer);

	ABaseCharacter::OnDeath_Implementation();
}

void APlayerCharacter::MoveForward(float _val)
{
	if (Controller && _val != 0.0f)
	{
		const FRotator tempRotation = GetActorRotation();
		FVector tempDirection = FRotationMatrix(tempRotation).GetScaledAxis(EAxis::X);
		AddMovementInput(tempDirection, _val);
	}
}

void APlayerCharacter::MoveRight(float _val)
{
	if (_val != 0.0f)
	{
		const FRotator tempRotation = GetActorRotation();
		FVector tempDirection = FRotationMatrix(tempRotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(tempDirection, _val);
	}
}

void APlayerCharacter::SwitchShoulder()
{
	if (rightShoulder)
	{
		springArmComp->SocketOffset = FVector(0, 100, 20);
		springArmComp->TargetOffset = FVector(0, 0, 55);
		rightShoulder = false;
	}
	else
	{
		springArmComp->SocketOffset = FVector(0, -100, 20);
		springArmComp->TargetOffset = FVector(0, 0, 55);
		rightShoulder = true;
	}
}

void APlayerCharacter::WallCheck()
{
	FRotator tempActorRotation = this->GetActorRotation();
	FRotator tempActorYaw(0, tempActorRotation.Yaw, 0);
	//This line needs checking!!!
	FVector tempForwardVector = tempActorYaw.Vector();
	FVector tempActorLocation = this->GetActorLocation();

	FVector ForwardCheckVector = tempForwardVector * 100;
	ForwardCheckVector = tempActorLocation + ForwardCheckVector;

	//https://goo.gl/pN6vYF
	FCollisionQueryParams  TraceParams = FCollisionQueryParams(FName(TEXT("RV_TRACE")), false, this);
	TraceParams.bTraceComplex = false;
	TraceParams.bTraceAsyncScene = false;
	TraceParams.bReturnPhysicalMaterial = false;

	FHitResult RV_Hit(ForceInit);
	//Forward check
	GetWorld()->LineTraceSingle(
		RV_Hit,//result
		tempActorLocation,//start of line trace
		ForwardCheckVector,//end of line trace
		ECC_Visibility,//collision channel, maybe wrong
		TraceParams);
	if (RV_Hit.bBlockingHit)
	{
		wallTouch = true;
		wallJumpNormal = RV_Hit.ImpactNormal;
		UE_LOG(LogTemp, Warning, TEXT("Found hit"));
		return;
	}

	FVector RightCheckVector = tempForwardVector.RotateAngleAxis(90, FVector(0, 0, 1));
	RightCheckVector = RightCheckVector * 100;
	RightCheckVector = tempActorLocation + RightCheckVector;
	//Right check
	GetWorld()->LineTraceSingle(
		RV_Hit,//result
		tempActorLocation,//start of line trace
		RightCheckVector,//end of line trace
		ECC_Visibility,//collision channel, maybe wrong
		TraceParams);

	if (RV_Hit.bBlockingHit)
	{
		wallTouch = true;
		wallJumpNormal = RV_Hit.ImpactNormal;
		UE_LOG(LogTemp, Warning, TEXT("Found hit"));
		return;
	}

	FVector LeftCheckVector = tempForwardVector.RotateAngleAxis(270, FVector(0, 0, 1));
	LeftCheckVector = LeftCheckVector * 100;
	LeftCheckVector = tempActorLocation + LeftCheckVector;
	//Left check
	GetWorld()->LineTraceSingle(
		RV_Hit,//result
		tempActorLocation,//start of line trace
		LeftCheckVector,//end of line trace
		ECC_Visibility,//collision channel, maybe wrong
		TraceParams);

	if (RV_Hit.bBlockingHit)
	{
		wallTouch = true;
		wallJumpNormal = RV_Hit.ImpactNormal;
		UE_LOG(LogTemp, Warning, TEXT("Found hit"));
		return;
	}
	else
		wallTouch = false;
	UE_LOG(LogTemp, Warning, TEXT("Did not find hit"));
}

void APlayerCharacter::WallJump()
{
	WallJumpServer();
	//if (wallTouch == true)
	//{
	//	if (this->CharacterMovement->IsFalling())
	//	{
	//		//To disable ever increasing falling speed
	//		this->CharacterMovement->Velocity = FVector(0, 0, 0);

	//		FRotator temp = GetActorRotation();
	//		FRotator tempControllerRotation = GetActorRotation();
	//		FRotator tempControllerYaw(0, tempControllerRotation.Yaw, 0);
	//		//Wall jump is adjusted using these 3
	//		FVector tempForwardVector = tempControllerYaw.Vector() * 500.0f;
	//		FVector tempResult = tempForwardVector.MirrorByVector(wallJumpNormal) * 2.5f;
	//		tempResult = tempResult + FVector(0.0f, 0.0f, 800.0f);

	//		LaunchCharacter(tempResult, false, false);
	//		//Hox!
	//		LoseStamina(25.0f);
	//	}
	//}
}

bool APlayerCharacter::WallJumpServer_Validate()
{
			return true;
}

void APlayerCharacter::WallJumpServer_Implementation()
{
	if (wallTouch)
	{
		if (this->CharacterMovement->IsFalling())
		{
			//To disable ever increasing falling speed
			FVector tempVel = CharacterMovement->Velocity;
			CharacterMovement->Velocity = FVector(tempVel.X, tempVel.Y, 0);

			FVector tempResult = wallJumpNormal * 500.0f;



			LaunchCharacter(tempResult, false, false);
			//Hox!
			LoseStamina(25.0f);
		}
	}
}

void APlayerCharacter::InputDash()
{
	ServerDash(GetInputAxisValue("MoveForward") , GetInputAxisValue("MoveRight"));
}

bool APlayerCharacter::ServerDash_Validate(float _inputForward, float _inputRight)
{
	return true;
}

void APlayerCharacter::ServerDash_Implementation(float _inputForward, float _inputRight)
{
	if (IsMoveInputIgnored())
		return;

	if (this->CharacterMovement->IsFalling())
	{
		dash_Multiplier = 1000;
	}
	else
		dash_Multiplier = 5000;

	FVector tempForward = this->GetActorForwardVector();
	FVector tempRight = this->GetActorRightVector();
	FVector tempForwardResult = tempForward * _inputForward;
	FVector tempRightResult = tempRight * _inputRight;
	FVector tempResult = tempForwardResult + tempRightResult;
	tempResult.Normalize();
	tempResult = tempResult * dash_Multiplier;
	LaunchCharacter(tempResult, false, false);
	LoseStamina(20.0f);

	if (dashSound)
	{
		PlaySound(dashSound);
	}
}

void APlayerCharacter::PlaySound_Implementation(USoundCue* _component)
{
	audioComp->SetSound(_component);
	audioComp->Play();
}

void APlayerCharacter::OpenTeamChat()
{
	AHUD *hud = Cast<APlayerController>(Controller)->GetHUD();
	APlayerHud *playerhud = Cast<APlayerHud>(hud);


}
void APlayerCharacter::OpenAllChat()
{
	AHUD *hud = Cast<APlayerController>(Controller)->GetHUD();
	APlayerHud *playerhud = Cast<APlayerHud>(hud);

}
void APlayerCharacter::WeaponSlot1()
{
	int tempLastWeapon = currentWeapon;
	currentWeapon = 0;
	if (currentWeapon == tempLastWeapon)
		return;
	ServerSwitchWeapon(currentWeapon, tempLastWeapon);
	//TODO make these into one function
}
void APlayerCharacter::WeaponSlot2()
{
	int tempLastWeapon = currentWeapon;
	currentWeapon = 1;
	if (currentWeapon == tempLastWeapon)
		return;
	ServerSwitchWeapon(currentWeapon, tempLastWeapon);
}
void APlayerCharacter::WeaponSlot3()
{
	int tempLastWeapon = currentWeapon;
	currentWeapon = 2;
	if (currentWeapon == tempLastWeapon)
		return;
	ServerSwitchWeapon(currentWeapon, tempLastWeapon);
}
void APlayerCharacter::WeaponSlot4()
{
	int tempLastWeapon = currentWeapon;
	currentWeapon = 3;
	if (currentWeapon == tempLastWeapon)
		return;
	ServerSwitchWeapon(currentWeapon, tempLastWeapon);
}