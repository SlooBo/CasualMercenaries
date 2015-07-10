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
	ADestructibleObject(const FObjectInitializer& FOI);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//Called when game ends
	virtual void EndPlay(const EEndPlayReason::Type _endPlayReason) override;

	UFUNCTION()
	void OnMyDestructibleHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "ObjectCondition")
		virtual void TakeDamage(float _damage);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(EditAnywhere, Replicated, Category = "ObjectCondition")
		float health;
	
	UPROPERTY(EditAnywhere, Replicated, Category = "ObjectCondition")
		float healthMax;

	UPROPERTY(VisibleDefaultsOnly, Category = "ObjectCondition")
	UStaticMeshComponent* meshComponent;

	UPROPERTY(EditAnywhere, Category = "ObjectCondition")
		UMaterial* normalMaterial;

	UPROPERTY(EditAnywhere, Category = "ObjectCondition")
	UMaterialInterface* invisibleMaterial;
private:

	void CheckStatus();
	void Respawn();
	void GoInvisible();

	FTimerHandle respawnTimerHandle;
	bool timerActive;

};
