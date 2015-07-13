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
	//virtual void BeginPlay() override;
	//had to be changed so blueprint can run


	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	//Called when game ends
	virtual void EndPlay(const EEndPlayReason::Type _endPlayReason) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(Reliable, NetMulticast, BlueprintCallable, Category = "PlayerCondition")
	virtual void SetState(CHARACTER_STATE _state) override;

	/************************************************************************/
	/* Movement                                                             */
	/************************************************************************/

	virtual void MoveForward(float _val);

	virtual void MoveRight(float _val);
	//UFUNCTION(BlueprintCallable)
	void WallJump();
	UFUNCTION(Reliable, Server, WithValidation)
	void WallJumpServer();
	//UFUNCTION(BlueprintCallable, Category = "Player movement action")
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerDash(float _inputForward, float _inputRight);

	UFUNCTION(Reliable, NetMulticast)
	void PlaySound(USoundCue* _component);

	/* Client mapped to Input */
	void OnStartJump();

	/* Client mapped to Input */
	void OnStopJump();

	/* Is character currently performing a jump action. Resets on landed.  */
	UPROPERTY(Transient, Replicated)
	bool bIsJumping;


	/************************************************************************/
	/* Life And Death                                                       */
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	virtual void TakeDamage(float _damage, DAMAGE_TYPE _type, ACMPlayerController* killer = NULL) override;

	virtual void OnDeath_Implementation(ACMPlayerController* killer = NULL) override;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerOnDeath(ACMPlayerController* killer = NULL);

	/************************************************************************/
	/* Camera                                                               */
	/************************************************************************/

	UCameraComponent* GetCamera(){ return cameraComp; };

	/************************************************************************/
	/* Utility                                                              */
	/************************************************************************/

	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ChangeShirtColor(FLinearColor color);

	virtual bool IsNetRelevantFor(const AActor* realViewer, const AActor* viewTarget, const FVector& srcLocation) const override;

private:

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* springArmComp;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* cameraComp;

	UPROPERTY(VisibleDefaultsOnly,Category = "Camera")
	bool rightShoulder;
	void SwitchShoulder();

	UPROPERTY(VisibleDefaultsOnly)
	UAudioComponent* audioComp;

	UPROPERTY(EditDefaultsOnly, Category = "Player|Effects")
	USoundCue* dashSound;

	//Status timers and boold
	FTimerHandle stunTimerHandle;
	bool canLook;
	FTimerHandle rootTimerHandle;
	bool canWalk;
	void RestoreActivity();


	UPROPERTY(VisibleDefaultsOnly, Category = "Player actions")
	bool wallTouch;
	UPROPERTY(VisibleDefaultsOnly, Category = "Player actions")
	float dash_Multiplier;
	UPROPERTY(VisibleDefaultsOnly, Category = "Player actions")
	FVector wallJumpNormal;

	void WallCheck();
	void InputDash();

	int rounds;
};
