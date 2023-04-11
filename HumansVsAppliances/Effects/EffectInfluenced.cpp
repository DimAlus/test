// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectInfluenced.h"

#include "Effect.h"
#include "EffectComponent.h"
#include "UObject/Object.h"

#include <string>
// Add default functionality here for any IEffectInfluenced functions that are not pure virtual.


//void IEffectInfluenced::AddAllEffects_Implementation(TArray<UEffect*>& effects) {
//	for (int i = 0; i < effects.Num(); i++)
//		this->AddEffect(effects[i]);
//}

UEffectComponent* IEffectInfluenced::CreateComponentFromEffect(UEffect* effect, AActor* owner, AActor* other, float radius) {
	/*UEffectComponent* component = owner->CreateDefaultSubobject<UEffectComponent, UEffectComponent>(FName());
	ConstructObject<UEffectComponent>();
	component->Init(effect, other, isRadianal);
	return component;*/
	static int numEff = 0;
	FString name = "EFFECT_" + FString::FromInt(numEff++);
	UEffectComponent* NewComponent = NewObject<UEffectComponent>(owner, FName(name));//TEXT(effect->effectType));

	NewComponent->RegisterComponent();
	//NewComponent->CreateCluster();
	//NewComponent->OnComponentCreated(); // Might need this line, might not.
	NewComponent->OnCreatedFromReplication(); // Might need this line, might not.

	//NewComponent->AttachTo(owner->GetRootComponent(), NAME_None);
	//USceneComponent::AttachToComponent
	NewComponent->InitializeComponent(); // Might need this line, might not.
	NewComponent->Init(effect, other, radius);
	return NewComponent;
}