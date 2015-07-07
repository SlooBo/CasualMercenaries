// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DestructibleObject.generated.h"

UCLASS()
class CASUALMERCENARIES_API ADestructibleObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructibleObject();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//Called when game ends
	virtual void EndPlay(const EEndPlayReason::Type _endPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "ObjectCondition")
		virtual void TakeDamage(float _damage);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _outLifetimeProps) const;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "ObjectCondition")
		float health;
	
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "ObjectCondition")
		float healthMax;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "ObjectCondition")
		USkeletalMeshComponent* mesh;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "ObjectCondition")
		UMaterialInterface normalMaterial;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "ObjectCondition")
	UMaterialInterface invisibleMaterial;
private:

	void CheckStatus();
	void Respawn();
	void GoInvisible();

	FTimerHandle respawnTimerHandle;

};
