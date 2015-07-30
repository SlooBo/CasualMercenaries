// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DestructibleObject.generated.h"

#pragma warning(disable : 4263) // C4263: member function does not override any base class virtual member function
#pragma warning(disable : 4264) // C4264: no override available for virtual member function from base 'class'; function is hidden

UCLASS()
class CASUALMERCENARIES_API ADestructibleObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructibleObject(const FObjectInitializer& FOI);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Called when game ends
	virtual void EndPlay(const EEndPlayReason::Type _endPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "ObjectCondition")
		virtual void TakeDamage(float _damage);

	UPROPERTY(VisibleDefaultsOnly, Category = "ObjectCondition")
	UStaticMeshComponent* meshComponent;

	UPROPERTY(EditAnywhere, Category = "ObjectCondition")
		UMaterial* normalMaterial;

	UPROPERTY(EditAnywhere, Category = "ObjectCondition")
	UMaterialInterface* invisibleMaterial;

	UFUNCTION(Reliable, NetMulticast)
	void Respawn();
	UFUNCTION(Reliable, NetMulticast)
	void GoInvisible();

private:

	void CheckStatus();

	FTimerHandle respawnTimerHandle;
	bool timerActive;

	float health;
	float healthMax;
};
