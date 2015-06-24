// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class CASUALMERCENARIES_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/************************************************************************/
	/* Character condition                                                  */
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		float GetHealthMax() const;
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		float GetStamina() const;
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		float GetStaminaMax() const;
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		float GetArmor() const;
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		float GetArmorMax() const;


	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		void AddHealth(float _health);

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		void TakeDamage(float _damage, APlayerController* killer = NULL);

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		void AddStamina(float _stamina);

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		void LoseStamina(float _fatigue);

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		void AddArmor(float _armor);

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
		void LoseArmor(float _damage);

protected:
	UPROPERTY(VisibleAnywhere, Replicated, Category = "PlayerCondition")
	float health;
	UPROPERTY(VisibleAnywhere, Category = "PlayerCondition")
	float health_Max;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "PlayerCondition")
	float stamina;
	UPROPERTY(VisibleAnywhere, Category = "PlayerCondition")
	float stamina_Max;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "PlayerCondition")
	float armor;
	UPROPERTY(VisibleAnywhere, Category = "PlayerCondition")
	float armor_Max;

	/************************************************************************/
	/* Damage & Death                                                       */
	/************************************************************************/

	/* Take damage & handle death */

	virtual void OnDeath(APlayerController* damageSource = NULL) { };

	/*virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser);

	virtual void FellOutOfWorld(const class UDamageType& DmgType) override;*/

	void SetRagdollPhysics();

	//virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	//void ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	///* Holds hit data to replicate hits and death to clients */
	//UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	//struct FTakeHitInfo LastTakeHitInfo;

	//UFUNCTION()
	//void OnRep_LastTakeHitInfo();
};
