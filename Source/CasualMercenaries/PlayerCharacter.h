// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseCharacter.h"
#include "Weapon.h"
#include "Inventory.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CASUALMERCENARIES_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:

	// Sets default values for this character's properties
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	/************************************************************************/
	/* Movement                                                             */
	/************************************************************************/

	virtual void MoveForward(float _val);

	virtual void MoveRight(float _val);
	//UFUNCTION(BlueprintCallable)
	void WallJump();
	//UFUNCTION(BlueprintCallable, Category = "Player movement action")
	void Dash(float _inputForward, float _inputRight);

	/* Client mapped to Input */
	void OnStartJump();

	/* Client mapped to Input */
	void OnStopJump();

	virtual void OnDeath() override;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerOnDeath();


	/* Is character currently performing a jump action. Resets on landed.  */
	UPROPERTY(Transient, Replicated)
		bool bIsJumping;

	void AddWeapon(AWeapon* _weapon);
	void ChangeUITest();

	void UseWeapon1();
	void UseWeapon1Release();
	void UseWeapon2();
	void SwitchWeaponUp();
	void SwitchWeaponDown();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerUseWeapon1();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSwitchWeaponUp();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSwitchWeaponDown();


private:

	Inventory inventory;
	int currentWeapon;
	FVector fuckthisshit;
	FString nickName;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* springArmComp;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* cameraComp;

	bool rightShoulder;
	void SwitchShoulder();

	void WallCheck();
	void InputDash();

	bool wallTouch;
	float dash_Multiplier;
	FVector wallJumpNormal;

	void OpenTeamChat();
	void OpenAllChat();
};
