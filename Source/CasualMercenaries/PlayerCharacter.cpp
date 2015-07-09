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

	//Audio component initialization
	audioComp = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComp"));
	audioComp->bAutoActivate = false;
	audioComp->bAutoDestroy = false;
	audioComp->AttachParent = GetRootComponent();

	//	CharacterMesh
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(TEXT("SkeletalMesh'/Game/Game/PlayerCharacters/PlayerCharacter_Ver2/MESH_PlayerCharacter.MESH_PlayerCharacter'"));
	Mesh->SetSkeletalMesh(MeshObj.Object);
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MateriaObj(TEXT("MaterialInstanceConstant'/Game/Game/PlayerCharacters/PlayerCharacter_Ver2/MAT_Playercharacter_Inst.MAT_Playercharacter_Inst'"));
	Mesh->SetMaterial(0, MateriaObj.Object);
	const ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBuleprintObj(TEXT("AnimBlueprint'/Game/Game/PlayerCharacters/PlayerCharacter_Ver2/APB_PlayerCharacter.APB_PlayerCharacter'"));
	Mesh->AnimBlueprintGeneratedClass = AnimBuleprintObj.Object->GetAnimBlueprintGeneratedClass();

	// Values from blueprint
	CapsuleComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.3f));
	CapsuleComponent->bVisible = false;
	Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -83.0f));
	Mesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	Mesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.6f));


	rightShoulder = false;

	health_Max = 100.0f;
	health = health_Max;

	stamina_Max = 100.0f;
	stamina = stamina_Max;

	armor_Max = 100.0f;
	armor = armor_Max;

	wallTouch = false;
	dash_Multiplier = 0;

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

	WallCheck();
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type _endPlayReason)
{
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

void APlayerCharacter::OnStartJump()
{
	if (IsMoveInputIgnored())
		return;

	if (state == CHARACTER_STATE::ALIVE)
	bPressedJump = true;

}

void APlayerCharacter::OnStopJump()
{

}

void APlayerCharacter::TakeDamage(float _damage, DAMAGE_TYPE _type, ACMPlayerController* damageSource)
{
	if (!IsAlive() || GetController() == NULL)
		return;

	health = health - _damage;

	switch (_type)
	{
	case DAMAGE_TYPE::NORMAL:
		break;
	case DAMAGE_TYPE::STUN:
		canLook = false;
		springArmComp->bUsePawnControlRotation = false;
		GetWorld()->GetTimerManager().SetTimer(stunTimerHandle, this, &APlayerCharacter::RestoreActivity, 3.0f, false);
		SetState(CHARACTER_STATE::STUNNED);
		break;
	case DAMAGE_TYPE::ROOT:
		canWalk = false;
		GetWorld()->GetTimerManager().SetTimer(stunTimerHandle, this, &APlayerCharacter::RestoreActivity, 3.0f, false);
		SetState(CHARACTER_STATE::ROOTED);
		break;
	default:
		break;
	}

	//Temp fix
	if (!IsAlive())
		OnDeath(Cast<ACMPlayerController>(damageSource));
}

void APlayerCharacter::RestoreActivity()
{
	SetState(CHARACTER_STATE::ALIVE);
	canWalk = true;
	canLook = true;
	springArmComp->bUsePawnControlRotation = true;
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

			FVector tempResult = wallJumpNormal * 700.0f + FVector(0.0f,0.0f,600.0f);

			LaunchCharacter(tempResult, false, false);
			//Hox!
			LoseStamina(25.0f);
		}
	}
}

void APlayerCharacter::InputDash()
{
	if (state == CHARACTER_STATE::ALIVE)
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

bool APlayerCharacter::ChangeShirtColor_Validate(FLinearColor color)
{
	return true;
}
void APlayerCharacter::ChangeShirtColor_Implementation(FLinearColor color)
{
	UMaterialInstanceDynamic *dynamicMesh = Mesh->CreateDynamicMaterialInstance(0, Mesh->GetMaterial(0));
	dynamicMesh->SetVectorParameterValue("ShirtColour", color);
	Mesh->SetMaterial(0, dynamicMesh);
}

bool APlayerCharacter::IsNetRelevantFor(const AActor* realViewer, const AActor* viewTarget, const FVector& srcLocation) const
{
	bool relevant = Super::IsNetRelevantFor(realViewer, viewTarget, srcLocation);

	// deny actor updates for ghost characters
	if (viewTarget != this && Cast<AGhostCharacter>(viewTarget) != NULL)
		relevant = false;

	return relevant;
}