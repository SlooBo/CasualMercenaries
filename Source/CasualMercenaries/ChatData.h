// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ChatData.generated.h"

UCLASS()
class CASUALMERCENARIES_API AChatData : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChatData();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddText(const FString& text);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	UFUNCTION()
	TArray<FString> getChatMessages();
private:
	UPROPERTY(Replicated)
	TArray<FString> publicChat;
	
};
