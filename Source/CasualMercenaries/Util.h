// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define STRINGIFY(str) L#str

/**
 * 
 */
class CASUALMERCENARIES_API Util
{
public:
	static class APlayerCharacter* GetLocalPlayer(UWorld* world);
	static class APlayerCharacter* GetPlayerCharacter(APlayerController* player);
	static int32 GetNumPlayers(UWorld* world);
	//from https://wiki.unrealengine.com/Dynamic_Load_Object
	template <typename ObjClass>
	static FORCEINLINE ObjClass* LoadObjFromPath(const FName& Path)
	{
		if (Path == NAME_None) return NULL;
		//~

		return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
	}
	static FORCEINLINE FName GetObjPath(const UObject* Obj)
	{
		if (!Obj) return NAME_None;
		if (!Obj->IsValidLowLevel()) return NAME_None;
		//~

		FStringAssetReference ThePath = FStringAssetReference(Obj);

		if (!ThePath.IsValid()) return NAME_None;

		//The Class FString Name For This Object
		FString Str = Obj->GetClass()->GetDescription();

		Str += "'";
		Str += ThePath.ToString();
		Str += "'";

		return FName(*Str);
	}

	template <typename T>
	static FORCEINLINE T* RandomFromList(const TArray<T*> list)
	{
		if (list.Num() == 0)
			return NULL;

		int i = FMath::RandRange(0, list.Num() - 1);
		return list[i];
	}
};