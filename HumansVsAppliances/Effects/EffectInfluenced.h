// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Effect.h"
#include "EffectComponent.h"

#include "EffectInfluenced.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEffectInfluenced : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HUMANSVSAPPLIANCES_API IEffectInfluenced
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	void AddAllEffects(TArray<UEffect*>& effects, AActor* other, float radius);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	void AddEffect(UEffect* effect, AActor* other, float radius);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	void AddEffectFromMeta(UEffectsMeta* effect, AActor* other, float radius);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Effects)
	void AddEffectComponent(UEffectComponent* component);

	static UEffectComponent* CreateComponentFromEffect(UEffect* effect, AActor* owner, AActor* other, float radius);


/*----------------------------  EFFECT INFLUENCE  ----------------------------*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = EffectsInfluence)
	void ChangeSpeed(float speed);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = EffectsInfluence)
	void ChangeHealth(float health, AActor* other);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = EffectsInfluence)
	void ChangeMaxHealth(float health, AActor* other);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = EffectsInfluence)
	void ChangeProtection(float health);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = EffectsInfluence)
	void AddWeapon(const FString& name);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = EffectsInfluence)
	void AddProjectile(int count);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = EffectsInfluence)
	void AddMagazineSize(int count);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = EffectsInfluence)
	void SetParalisys(bool isParalisys);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = EffectsInfluence)
	bool IsParalisys();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = EffectsInfluence)
	void Repuls(float power, FVector dir);

};
