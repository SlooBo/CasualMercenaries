// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "UnrealNetwork.h"
#include "PlayerCharacter.h"
#include "GhostCharacter.h"
#include "PlayerHud.h"
#include "CMGameMode.h"
#include "UberWeihmacher.h"
#include "MashineGun.h"
#include "RocketLauncher.h"
#include "PomegranadeLauncher.h"
#include "MUDbuster.h"
#include "WaspNestCudgel.h"
#include "CMPlayerController.h"
#include "CMPlayerState.h"
#include "Hook.h"
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
	springArmComp->SocketOffset = FVector(0, 100, 20);
	springArmComp->TargetOffset = FVector(0, 0, 100);
	springArmComp->bUsePawnControlRotation = true;
	springArmComp->TargetArmLength = 400;
	springArmComp->AttachParent = GetRootComponent();

	cameraComp = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
	cameraComp->AttachParent = springArmComp;
	cameraComp->SetRelativeLocation(FVector(130, 0, 0));
	//Audio component initialization
	audioComp = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComp"));
	audioComp->bAutoActivate = false;
	audioComp->bAutoDestroy = false;
	audioComp->AttachParent = GetRootComponent();

	


	//	CharacterMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/PlayerCharacters/ver7/Character_updatedanimations.Character_updatedanimations'"));
	GetMesh()->SetSkeletalMesh(MeshObj.Object);
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MateriaObj(TEXT("MaterialInstanceConstant'/Game/Game/PlayerCharacters/ver7/MAT_PlayerCharacter_updated_Inst.MAT_PlayerCharacter_updated_Inst'"));
	GetMesh()->SetMaterial(0, MateriaObj.Object);
	GetMesh()->SetMaterial(1, MateriaObj.Object);
	const ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBuleprintObj(TEXT("AnimBlueprint'/Game/Game/PlayerCharacters/ver7/apb_test.apb_test'"));
	GetMesh()->AnimBlueprintGeneratedClass = AnimBuleprintObj.Object->GetAnimBlueprintGeneratedClass();

	// Values from blueprint
	//CapsuleComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.3f));
	GetCapsuleComponent()->bVisible = false;
	GetMesh()->SetRelativeLocation(FVector(-9.999983f, -0.000022f, -84.614967f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));

	rightShoulder = false;

	health_Max = 100.0f;
	health = health_Max;

	stamina_Max = 100.0f;
	stamina = stamina_Max;

	armor_Max = 100.0f;
	armor = armor_Max;

	staminaIncrease = 10.0f;

	wallTouch = false;
	dash_Multiplier = 0;

	dashing = false;

	//SetCurrentWeapon(0);

	state = CHARACTER_STATE::ALIVE;
	rounds = 0;
	canLook = true;
	canWalk = true;

	bReplicates = true;
	/// pleasant surprise 
}


// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!HasAuthority())
		return;
	StaminaRegenServer(DeltaTime); 
	WallCheck();
	UpdateDash();
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UMaterialInstanceDynamic *dynamicMesh = GetMesh()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterial(0));
	GetMesh()->SetMaterial(0, dynamicMesh);
	GetMesh()->SetMaterial(1, dynamicMesh);

	if (Role < ROLE_Authority)
		ChangeShirtColor(shirtColor);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type _endPlayReason)
{
	Super::EndPlay(_endPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
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
}

void APlayerCharacter::StaminaRegenServer(float DeltaTime)
{
	stamina+=DeltaTime * staminaIncrease ;
	if (stamina > 100)
		stamina = 100.0f;
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
	DOREPLIFETIME(APlayerCharacter, shirtColor);
};

void APlayerCharacter::OnStartJump()
{
	if (IsMoveInputIgnored())
		return;

	if (state == CHARACTER_STATE::ALIVE)
	bPressedJump = true;
	ACMPlayerController *controller = Cast<ACMPlayerController>(GetController()); 
	AHook *hook = Cast<AHook>(controller->GetInventory().GetWeapon(controller->GetInventory().currentWeapon));
	if (hook != nullptr && hook->hooked)
		hook->ReleaseHook();

}

void APlayerCharacter::OnStopJump()
{

}

void APlayerCharacter::TakeDamage(float _damage, DAMAGE_TYPE _type, ACMPlayerController* damageSource)
{
	if (!IsAlive() || GetController() == NULL)
		return;

	switch (_type)
	{
	case DAMAGE_TYPE::NORMAL:
		if (bulletHitSound)
		{
			PlaySound(bulletHitSound);
		}
		break;
	case DAMAGE_TYPE::STUN:
		SetState(CHARACTER_STATE::STUNNED);
		break;
	case DAMAGE_TYPE::ROOT:
		SetState(CHARACTER_STATE::ROOTED);
		break;
	default:
		break;
	}

	Super::TakeDamage(_damage, _type, damageSource);
}

void APlayerCharacter::SetState_Implementation(CHARACTER_STATE _state)
{
	state = _state;

	ACMPlayerController* pc = Cast<ACMPlayerController>(Controller);

	switch (state)
	{
	case CHARACTER_STATE::STUNNED:
		canLook = false;
		springArmComp->bUsePawnControlRotation = false;
		GetWorld()->GetTimerManager().SetTimer(stunTimerHandle, this, &APlayerCharacter::RestoreActivity, 3.0f, false);
		break;
	case CHARACTER_STATE::ROOTED:
		canWalk = false;
		if (pc != NULL)
			pc->SetIgnoreMoveInput(true);
		GetWorld()->GetTimerManager().SetTimer(stunTimerHandle, this, &APlayerCharacter::RestoreActivity, 3.0f, false);
		break;
	case CHARACTER_STATE::FROZEN:
		if (pc != NULL)
			pc->SetIgnoreMoveInput(true);
		canWalk = false;
		canLook = false;
		break;
	case CHARACTER_STATE::SHOPPING:
	{
		if (pc != NULL)
		{
			pc->SetIgnoreMoveInput(true);
			canLook = false;
			canWalk = false;
		}
	}
	default:
		if (pc != NULL)
			pc->SetIgnoreMoveInput(false);
		canLook = true;
		canWalk = true;
		break;
	}

}

void APlayerCharacter::RestoreActivity()
{
	SetState(CHARACTER_STATE::ALIVE);
	springArmComp->bUsePawnControlRotation = true;
}

void APlayerCharacter::FellOutOfWorld(const class UDamageType& DmgType)
{
	OnDeath(NULL);
}

void APlayerCharacter::OnDeath_Implementation(ACMPlayerController* killer)
{
	if (Role >= ROLE_AutonomousProxy)
		ServerOnDeath(killer);
}

bool APlayerCharacter::ServerOnDeath_Validate(ACMPlayerController* killer)
{
	return true;
}

void APlayerCharacter::ServerOnDeath_Implementation(ACMPlayerController* killer)
{
	ACMGameMode* gameMode = Cast<ACMGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	ACMPlayerController* playerController = Cast<ACMPlayerController>(GetController());
	if (gameMode != NULL && playerController != NULL)
		gameMode->OnPlayerDeath(playerController, killer);

	ABaseCharacter::OnDeath_Implementation();
}

void APlayerCharacter::MoveForward(float _val)
{
	if (canLook && canWalk)
	{
		if (Controller && _val != 0.0f)
		{
			const FRotator tempRotation = GetActorRotation();
			FVector tempDirection = FRotationMatrix(tempRotation).GetScaledAxis(EAxis::X);
			AddMovementInput(tempDirection, _val);
		}
	}
}

void APlayerCharacter::MoveRight(float _val)
{
	if (canLook && canWalk)
	{
		if (_val != 0.0f)
		{
			const FRotator tempRotation = GetActorRotation();
			FVector tempDirection = FRotationMatrix(tempRotation).GetScaledAxis(EAxis::Y);
			AddMovementInput(tempDirection, _val);
		}
	}
}

void APlayerCharacter::SwitchShoulder_Implementation()
{
	SwitchShoulderServer();
	UpdateShoulder();
	rightShoulder = !rightShoulder;
}

bool APlayerCharacter::SwitchShoulderServer_Validate()
{
	return true;
}

void APlayerCharacter::SwitchShoulderServer_Implementation()
{
	rightShoulder = !rightShoulder;
}

void APlayerCharacter::UpdateShoulder()
{
	if (rightShoulder)
	{
		springArmComp->SocketOffset = FVector(0, 100, 20);
		springArmComp->TargetOffset = FVector(0, 0, 100);
	}
	else
	{
		springArmComp->SocketOffset = FVector(0, -100, 20);
		springArmComp->TargetOffset = FVector(0, 0, 100);
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
	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,//result
		tempActorLocation,//start of line trace
		ForwardCheckVector,//end of line trace
		ECC_Visibility,//collision channel, maybe wrong
		TraceParams);
	if (RV_Hit.bBlockingHit)
	{
		wallTouch = true;
		wallJumpNormal = RV_Hit.ImpactNormal;
		//UE_LOG(LogTemp, Warning, TEXT("Found hit"));
		return;
	}

	FVector RightCheckVector = tempForwardVector.RotateAngleAxis(90, FVector(0, 0, 1));
	RightCheckVector = RightCheckVector * 100;
	RightCheckVector = tempActorLocation + RightCheckVector;
	//Right check
	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,//result
		tempActorLocation,//start of line trace
		RightCheckVector,//end of line trace
		ECC_Visibility,//collision channel, maybe wrong
		TraceParams);

	if (RV_Hit.bBlockingHit)
	{
		wallTouch = true;
		wallJumpNormal = RV_Hit.ImpactNormal;
		//UE_LOG(LogTemp, Warning, TEXT("Found hit"));
		return;
	}

	FVector LeftCheckVector = tempForwardVector.RotateAngleAxis(270, FVector(0, 0, 1));
	LeftCheckVector = LeftCheckVector * 100;
	LeftCheckVector = tempActorLocation + LeftCheckVector;
	//Left check
	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,//result
		tempActorLocation,//start of line trace
		LeftCheckVector,//end of line trace
		ECC_Visibility,//collision channel, maybe wrong
		TraceParams);

	if (RV_Hit.bBlockingHit)
	{
		wallTouch = true;
		wallJumpNormal = RV_Hit.ImpactNormal;
		//UE_LOG(LogTemp, Warning, TEXT("Found hit"));
		return;
	}
	else
		wallTouch = false;
	//UE_LOG(LogTemp, Warning, TEXT("Did not find hit"));
}

void APlayerCharacter::WallJump()
{
	if (stamina >= 25)
	{
		WallJumpServer();
	}
}

bool APlayerCharacter::WallJumpServer_Validate()
{
	return true;
}

void APlayerCharacter::WallJumpServer_Implementation()
{
	if (wallTouch)
	{
		if (this->GetCharacterMovement()->IsFalling())
		{
			//To disable ever increasing falling speed
			FVector tempVel = GetCharacterMovement()->Velocity;
			GetCharacterMovement()->Velocity = FVector(tempVel.X, tempVel.Y, 0);

			FVector tempResult = wallJumpNormal * 700.0f + FVector(0.0f,0.0f,600.0f);

			LaunchCharacter(tempResult, false, false);
			//Hox!
			LoseStamina(25.0f);
		}
	}
}

void APlayerCharacter::InputDash()
{
	if (stamina >= 25)
	{
		ServerDash(GetInputAxisValue("MoveForward"), GetInputAxisValue("MoveRight"));
	}
}

bool APlayerCharacter::ServerDash_Validate(float _inputForward, float _inputRight)
{
	return true;
}

void APlayerCharacter::ServerDash_Implementation(float _inputForward, float _inputRight)
{
	if (IsMoveInputIgnored())
		return;
	if (state != CHARACTER_STATE::ALIVE)
		return;

	FVector tempForward = this->GetActorForwardVector();
	FVector tempRight = this->GetActorRightVector();
	FVector tempForwardResult = tempForward * _inputForward;
	FVector tempRightResult = tempRight * _inputRight;
	FVector tempResult = tempForwardResult + tempRightResult;
	tempResult.Normalize();
	tempResult = tempResult * 650;

	if (dashSound)
	{
		PlaySound(dashSound);
	}

	FVector tempActorLocation = this->GetActorLocation();
	FVector tempLowLine = tempActorLocation + FVector(0, 0, -85);
	FVector tempHighLine = tempActorLocation + FVector(0, 0, 125);


	//https://goo.gl/pN6vYF
	FCollisionQueryParams  TraceParams = FCollisionQueryParams(FName(TEXT("RV_TRACE")), false, this);
	TraceParams.bTraceComplex = false;
	TraceParams.bTraceAsyncScene = false;
	TraceParams.bReturnPhysicalMaterial = false;

	FHitResult RV_Hit(ForceInit);
	//Forward check low
	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,//result
		tempLowLine,//start of line trace
		tempLowLine + tempResult,//end of line trace
		ECC_Visibility,//collision channel, maybe wrong
		TraceParams);
	if (RV_Hit.bBlockingHit)
	{
		dashing = true;
		LoseStamina(25.0f);
		dashEndLocation = RV_Hit.Location;
		canWalk = false;
		UE_LOG(LogTemp, Warning, TEXT("Hit wall"));
		return;
	}
	else
	{
		if (tempResult.Size() == 0)
		{
			GetWorld()->LineTraceSingleByChannel(
				RV_Hit,//result
				tempLowLine,//start of line trace
				tempLowLine - (tempForward * 650),//end of line trace
				ECC_Visibility,//collision channel, maybe wrong
				TraceParams);
			if (RV_Hit.bBlockingHit)
			{
				dashing = true;
				LoseStamina(25.0f);
				dashEndLocation = RV_Hit.Location;
				canWalk = false;
				UE_LOG(LogTemp, Warning, TEXT("Hit wall"));
				return;
			}

			GetWorld()->LineTraceSingleByChannel(
				RV_Hit,//result
				tempHighLine,//start of line trace
				tempHighLine - (tempForward * 650),//end of line trace
				ECC_Visibility,//collision channel, maybe wrong
				TraceParams);
			if (RV_Hit.bBlockingHit)
			{
				dashing = true;
				LoseStamina(25.0f);
				dashEndLocation = RV_Hit.Location;
				canWalk = false;
				UE_LOG(LogTemp, Warning, TEXT("Hit wall"));
				return;
			}


			dashing = true;
			LoseStamina(25.0f);
			dashEndLocation = tempActorLocation - (tempForward * 650 );
			canWalk = false;
			return;
		}
		//Forward high check
		GetWorld()->LineTraceSingleByChannel(
			RV_Hit,//result
			tempHighLine,//start of line trace
			tempHighLine + tempResult,//end of line trace
			ECC_Visibility,//collision channel, maybe wrong
			TraceParams);
		if (RV_Hit.bBlockingHit)
		{
			dashing = true;
			LoseStamina(25.0f);
			dashEndLocation = RV_Hit.Location;
			canWalk = false;
			UE_LOG(LogTemp, Warning, TEXT("Hit wall"));
			return;
		}

		dashing = true;
		LoseStamina(25.0f);
		dashEndLocation = tempActorLocation + tempResult;
		canWalk = false;
	}
}

bool APlayerCharacter::UpdateDash_Validate()
{
	return true;
}

void APlayerCharacter::UpdateDash_Implementation()
{
	if (dashing)
	{
		FVector tempActorLocation = this->GetActorLocation();
		FVector tempDistance = tempActorLocation - dashEndLocation;
		
		if (tempDistance.Size() < 100)
		{ 
			dashing = false;
			FVector tempVel = GetCharacterMovement()->Velocity;
			float tempZ = tempVel.Z;
			tempVel.Normalize();
			tempVel = tempVel * GetCharacterMovement()->MaxWalkSpeed;
			GetCharacterMovement()->Velocity = FVector(tempVel.X, tempVel.Y, tempZ);
		}
		else
		{
			if (GetCharacterMovement()->IsFalling())
			{
				FVector tempVel = -(tempActorLocation - dashEndLocation);
				tempVel.Normalize();
				GetCharacterMovement()->Velocity = tempVel * 3000;
				return;
			}
			FVector tempVel = -(tempActorLocation - dashEndLocation);
			tempVel.Normalize();
			GetCharacterMovement()->Velocity = tempVel * 8300;
		}
	}
	else
	{
		canWalk = true;
	}
}

void APlayerCharacter::PlaySound_Implementation(USoundCue* _component)
{
	audioComp->SetSound(_component);
	audioComp->Play();
}

void APlayerCharacter::ChangeShirtColor_Implementation(FLinearColor color)
{
	shirtColor = color;
	UMaterialInstanceDynamic *dynamicMesh = Cast<UMaterialInstanceDynamic>(GetMesh()->GetMaterial(0));
	if (dynamicMesh != NULL)
		dynamicMesh->SetVectorParameterValue("ShirtColour", shirtColor);
	/*UMaterialInstanceDynamic *dynamicMesh = GetMesh()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterial(0));
	dynamicMesh->SetVectorParameterValue("ShirtColour", shirtColor);
	GetMesh()->SetMaterial(0, dynamicMesh);
	GetMesh()->SetMaterial(1, dynamicMesh);*/
}

bool APlayerCharacter::IsNetRelevantFor(const AActor* realViewer, const AActor* viewTarget, const FVector& srcLocation) const
{
	bool relevant = Super::IsNetRelevantFor(realViewer, viewTarget, srcLocation);

	// deny actor updates for ghost characters
	if (viewTarget != this && Cast<AGhostCharacter>(viewTarget) != NULL)
		relevant = false;

	return relevant;
}
void APlayerCharacter::SetStaminaRate(float rate)
{
	staminaIncrease = rate;
}