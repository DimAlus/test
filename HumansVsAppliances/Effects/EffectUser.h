// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Effect.h"
#include "EffectUser.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEffectUser : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HUMANSVSAPPLIANCES_API IEffectUser
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	AActor* GetMyOwner();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	void LoadEffect(UEffectsMeta* effectMeta);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	void LoadAllEffects(TArray<UEffectsMeta*>& effectsMeta);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	int ApplyEffect(UEffect* effect, int toIndex);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	void ApplyAllEffects(TArray<UEffect*>& effects, int toIndex);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	void CancelEffectByIndex(int index);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	TArray<int> AddEffectsToWeapon(const TArray<UEffect*>& wepeffects);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	TArray<UEffect*> GetEffectsToWeapon();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	TArray<UEffect*> GetAllEffectsOnDistantion (float distantion, AActor* who);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	TArray<UEffect*> GetAllEffectsOnPoint (AActor* who);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	TArray<UEffect*> GetAllEffects (AActor* who);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	float MaxEffectRadius();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	bool IsPointDamage();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	void AddCoins(int coins);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	void StartEffectLoading();



	static float GetMaxEffectRadius(TArray<UEffect*>& effects);
	static TArray<UEffect*> GetAllEffectsOnDistantion(float distantion, TArray<UEffect*>& effects);
	static TArray<UEffect*> GetAllEffectsOnPoint(TArray<UEffect*>& effects);
	static TArray<UEffect*> FilterEffects(TArray<UEffect*> effects, AActor* who);
};
