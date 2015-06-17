// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "UnrealNetwork.h"
#include "PlayerCharacter.h"
#include "PlayerHud.h"
#include "Chat.h"
// Sets default values
APlayerCharacter::APlayerCharacter(const class FObjectInitializer& ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UCharacterMovementComponent* tempMoveComp = GetCharacterMovement();
	//Addjusting jump
	tempMoveComp->GravityScale = 1.0;
	tempMoveComp->JumpZVelocity = 600;
	tempMoveComp->AirControl = 0.5f;
	springArmComp = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraSpring"));
	//these 2 need finetuning
	springArmComp->SocketOffset = FVector(0, 35, 0);
	springArmComp->TargetOffset = FVector(0, 0, 55);
	springArmComp->bUsePawnControlRotation = true;
	springArmComp->AttachParent = GetRootComponent();

	cameraComp = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
	cameraComp->AttachParent = springArmComp;

	health_Max = 100.0f;
	health = health_Max;

	stamina_Max = 100.0f;
	stamina = stamina_Max;

	armor_Max = 100.0f;
	armor = armor_Max;

	wallTouch = false;
	dash_Multiplier = 0;

	nickName = "Noob";
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	WallCheck();
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

	InputComponent->BindAction("WallJump", IE_Pressed, this, &APlayerCharacter::WallJump);

	InputComponent->BindAction("AllChat", IE_Pressed, this, &APlayerCharacter::OpenAllChat);
	InputComponent->BindAction("TeamChat", IE_Pressed, this, &APlayerCharacter::OpenTeamChat);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Value is already updated locally, skip in replication step
	//DOREPLIFETIME_CONDITION(APlayerCharacter, value, COND_SkipOwner);

	//Replicated to every client, no special condition required
	//DOREPLIFETIME(APlayerCharacter, value);
};

void APlayerCharacter::AddWeapon(AWeapon* _weapon)
{

}
void APlayerCharacter::OnStartJump()
{
	bPressedJump = true;

	
}
void APlayerCharacter::OnStopJump()
{

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
		ECC_Pawn,//collision channel, maybe wrong
		TraceParams);
	if (RV_Hit.bBlockingHit)
	{
		wallTouch = true;
		wallJumpNormal = RV_Hit.ImpactNormal;
		UE_LOG(LogTemp, Warning, TEXT("Found hit"));
		return;
		
	}

	FVector RightCheckVector = tempForwardVector.RotateAngleAxis(90, FVector(0, 1, 0));
	RightCheckVector = RightCheckVector * 100;
	//Right check
	GetWorld()->LineTraceSingle(
		RV_Hit,//result
		tempActorLocation,//start of line trace
		RightCheckVector,//end of line trace
		ECC_Pawn,//collision channel, maybe wrong
		TraceParams);

	if (RV_Hit.bBlockingHit)
	{
		wallTouch = true;
		wallJumpNormal = RV_Hit.ImpactNormal;
		UE_LOG(LogTemp, Warning, TEXT("Found hit"));
		return;
	}

	FVector LeftCheckVector = LeftCheckVector.RotateAngleAxis(270, FVector(0, 1, 0));
	LeftCheckVector = LeftCheckVector * 100;
	//Left check
	GetWorld()->LineTraceSingle(
		RV_Hit,//result
		tempActorLocation,//start of line trace
		LeftCheckVector,//end of line trace
		ECC_Pawn,//collision channel, maybe wrong
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

			FRotator tempControllerRotation = GetControlRotation();
			FRotator tempControllerYaw(0, tempControllerRotation.Yaw, 0);
			//This line need checking!!!
			FVector tempForwardVector = tempControllerYaw.Vector();
			FVector tempResult = tempForwardVector.MirrorByVector(wallJumpNormal);
			tempResult = tempResult + FVector(0, 0, 1000);

			LaunchCharacter(tempResult, false, false);
			//Hox!
			LoseStamina(25.0f);
		}
	}
}

void APlayerCharacter::InputDash()
{
	FVector temp = GetLastMovementInputVector();
	Dash(500, temp.Y);
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
	FVector tempForwardResult = tempForward + _inputForward;
	FVector tempRightResult = tempRight + _inputRight;
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