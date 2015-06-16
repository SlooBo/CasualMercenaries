// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "UnrealNetwork.h"
#include "PlayerCharacter.h"

// Sets default values
APlayerCharacter::APlayerCharacter(const class FObjectInitializer& ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UCharacterMovementComponent* tempMoveComp = GetCharacterMovement();
	//Addjusting jump
	//tempMoveComp->GravityScale = ;
	//tempMoveComp->JumpZVelocity = ;

	springArmComp = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraSpring"));
	//these 2 need finetuning
	springArmComp->SocketOffset = FVector(0, 35, 0);
	springArmComp->TargetOffset = FVector(0, 0, 55);
	springArmComp->bUsePawnControlRotation = true;
	springArmComp->AttachParent = GetRootComponent();

	cameraComp = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera"));
	cameraComp->AttachParent = springArmComp;

	health_Max = 100;
	health = health_Max;

	stamina_Max = 100;
	stamina = stamina_Max;
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

	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::OnStopJump);



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
void APlayerCharacter::MoveForward(float Val)
{

}

void APlayerCharacter::MoveRight(float Val)
{

}