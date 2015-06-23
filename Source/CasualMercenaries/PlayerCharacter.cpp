// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "UnrealNetwork.h"
#include "PlayerCharacter.h"
#include "PlayerHud.h"
#include "Chat.h"
#include "ChatBroadcaster.h"
#include "CMGameMode.h"
#include "UberWeihmacher.h"
#include "MashineGun.h"
#include "RocketLauncher.h"
#include "PomegranadeLauncher.h"

// Sets default values
APlayerCharacter::APlayerCharacter(const class FObjectInitializer& ObjectInitializer)
{



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

	rightShoulder = false;

	health_Max = 100.0f;
	health = health_Max;

	stamina_Max = 100.0f;
	stamina = stamina_Max;

	armor_Max = 100.0f;
	armor = armor_Max;

	wallTouch = false;
	dash_Multiplier = 0;

	nickName = "Noob";

	currentWeapon = 0;



}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	UWorld* const World = GetWorld();

	APomeGranadeLauncher* pyssy4 = World->SpawnActor<APomeGranadeLauncher>(APomeGranadeLauncher::StaticClass(), this->GetActorLocation(), this->GetActorRotation());
	AddWeapon(pyssy4);

	ARocketLauncher* pyssy3 = World->SpawnActor<ARocketLauncher>(ARocketLauncher::StaticClass(), this->GetActorLocation(), this->GetActorRotation());
	AddWeapon(pyssy3);

	AUberWeihmacher* pyssy2 = World->SpawnActor<AUberWeihmacher>(AUberWeihmacher::StaticClass(), this->GetActorLocation(), this->GetActorRotation());
	AddWeapon(pyssy2);

	AMashineGun* pyssy = World->SpawnActor<AMashineGun>(AMashineGun::StaticClass(), this->GetActorLocation(), this->GetActorRotation());
	AddWeapon(pyssy);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//inventory.GetWeapon(currentWeapon)->SetActorLocation(this->GetActorLocation());
	//inventory.GetWeapon(currentWeapon)->SetActorRotation(FRotator(-cameraComp->GetComponentRotation().Pitch, cameraComp->GetComponentRotation().Yaw + 180, cameraComp->GetComponentRotation().Roll ));
	//inventory.GetWeapon(currentWeapon)->GetActorRotation().Pitch += 180.0f;
	WallCheck();

	//cameraComp->GetComponentRotation()
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
	InputComponent->BindAction("LeftMouseButtonReleased", IE_Released, this, &APlayerCharacter::UseWeapon1Release);
	InputComponent->BindAction("RightMouseButton", IE_Pressed, this, &APlayerCharacter::UseWeapon2);
	InputComponent->BindAction("MouseWheelUp", IE_Pressed, this, &APlayerCharacter::SwitchWeaponUp);
	InputComponent->BindAction("MouseWheelDown", IE_Pressed, this, &APlayerCharacter::SwitchWeaponDown);
}

void APlayerCharacter::AddWeapon(AWeapon* _weapon)
{
	inventory.AddWeaponToInventory(_weapon);
}

void APlayerCharacter::UseWeapon1()
{
	inventory.GetWeapon(currentWeapon)->PrimaryFunction(this);
}

void APlayerCharacter::UseWeapon1Release()
{
	inventory.GetWeapon(currentWeapon)->PrimaryFunction(this);
}

void APlayerCharacter::UseWeapon2()
{
	inventory.GetWeapon(currentWeapon)->SecondaryFunction();
}

void APlayerCharacter::SwitchWeaponUp()
{
	currentWeapon++;
	if (currentWeapon > 3)
		currentWeapon = 0;
}

void APlayerCharacter::SwitchWeaponDown()
{
	currentWeapon--;
	if (currentWeapon < 0)
		currentWeapon = 3;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Value is already updated locally, skip in replication step
	//DOREPLIFETIME_CONDITION(APlayerCharacter, value, COND_SkipOwner);

	//Replicated to every client, no special condition required
	//DOREPLIFETIME(APlayerCharacter, value);
};

void APlayerCharacter::OnStartJump()
{
	bPressedJump = true;

	
}
void APlayerCharacter::OnStopJump()
{

}

void APlayerCharacter::OnDeath()
{
	ServerOnDeath();
}


bool APlayerCharacter::ServerOnDeath_Validate()
{
	return true;
}

void APlayerCharacter::ServerOnDeath_Implementation()
{
	ACMGameMode* gameMode = static_cast<ACMGameMode*>(UGameplayStatics::GetGameMode(GetWorld()));
	APlayerController* playerController = static_cast<APlayerController*>(GetController());

	if (gameMode != NULL && playerController != NULL)
		gameMode->OnPlayerDeath(playerController);
}

bool APlayerCharacter::ServerAddChat_Validate(const FString& message)
{
	return true;
}

void APlayerCharacter::ServerAddChat_Implementation(const FString& message)
{
	ACMGameMode* gameMode = static_cast<ACMGameMode*>(UGameplayStatics::GetGameMode(GetWorld()));

	if (gameMode != NULL)
	{
		UChatBroadcaster* chat = gameMode->getServerChat();
		if (chat != NULL)
			chat->HandleMessage(message);
	}
}

void APlayerCharacter::ReceiveChat_Implementation(const FString& message)
{
	APlayerHud* playerHud = static_cast<APlayerHud*>(Controller->CastToPlayerController()->GetHUD());
	if (playerHud == NULL)
		return;

	UChat* chat = playerHud->GetChat();
	if (chat == NULL)
		return;

	chat->AddText(message);
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

	FVector RightCheckVector = tempForwardVector.RotateAngleAxis(90, FVector(1, 0, 0));
	RightCheckVector = RightCheckVector * 100;
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

	FVector LeftCheckVector = LeftCheckVector.RotateAngleAxis(270, FVector(1, 0, 0));
	LeftCheckVector = LeftCheckVector * 100;
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
}

void APlayerCharacter::WallJump()
{
	if (wallTouch == true)
	{
		if (this->CharacterMovement->IsFalling())
		{
			//To disable ever increasing falling speed
			this->CharacterMovement->Velocity = FVector(0, 0, 0);

			FRotator temp = GetActorRotation();
			FRotator tempControllerRotation = GetActorRotation();
			FRotator tempControllerYaw(0, tempControllerRotation.Yaw, 0);
			//Wall jump is adjusted using these 3
			FVector tempForwardVector = tempControllerYaw.Vector() * 500.0f;
			FVector tempResult = tempForwardVector.MirrorByVector(wallJumpNormal) * 2.5f;
			tempResult = tempResult + FVector(0.0f, 0.0f, 800.0f);

			LaunchCharacter(tempResult, false, false);
			//Hox!
			LoseStamina(25.0f);
		}
	}
}

void APlayerCharacter::InputDash()
{
	Dash(GetInputAxisValue("MoveForward") , GetInputAxisValue("MoveRight"));
}

void APlayerCharacter::Dash(float _inputForward, float _inputRight)
{
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
}

void APlayerCharacter::ChangeUITest()
{
	APlayerController* MyPC = Cast<APlayerController>(Controller);

	if (MyPC)
	{
		Cast<APlayerHud>(MyPC->GetHUD())->changeUIElement(MenuType::GAME_UI);
	}
}
void APlayerCharacter::OpenTeamChat()
{
	AHUD *hud = Cast<APlayerController>(Controller)->GetHUD();
	APlayerHud *playerhud = Cast<APlayerHud>(hud);
	UChat *chat = playerhud->GetChat();
	chat->OpenTeamChat();

}
void APlayerCharacter::OpenAllChat()
{
	AHUD *hud = Cast<APlayerController>(Controller)->GetHUD();
	APlayerHud *playerhud = Cast<APlayerHud>(hud);
	UChat *chat = playerhud->GetChat();
	chat->OpenAllChat();
}