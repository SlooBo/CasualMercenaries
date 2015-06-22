// Fill out your copyright notice in the Description page of Project Settings.

#include "CasualMercenaries.h"
#include "ChatData.h"
#include "UnrealNetwork.h"

// Sets default values
AChatData::AChatData()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	//bNetUseOwnerRelevancy = true;
	//this->SetOwner(this);
}

// Called when the game starts or when spawned
void AChatData::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChatData::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool AChatData::ServerAddText_Validate(const FString& text)
{
	
	return true;
}
void AChatData::ServerAddText_Implementation(const FString& text)
{
	publicChat.Add(text);
}
void AChatData::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AChatData, publicChat);
}
TArray<FString> AChatData::getChatMessages()
{
	return publicChat;
}